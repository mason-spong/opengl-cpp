#include "Application.h"
#include "Window.h" // Need full definition now
#include "Shader.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Camera.h" // Include Camera.h
#include "World.h"  // Include World.h
#include "MeshBuilder.h"
#include "MeshData.h"
#include "glm/geometric.hpp"
#include "glm/common.hpp"
#include "glm/trigonometric.hpp"

#include <cmath>
#include <cstddef>
#include <OpenGL/gl.h>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>
#include <utility>      // For std::pair
#include <stdexcept>    // For runtime_error
#include <chrono>       // For delta time calculation
#include <GLFW/glfw3.h> // Include the GLFW header
#include "stb_image.h"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

namespace
{
    constexpr bool kVSyncEnabled = false;
}

// --- Application Implementation ---

Application::Application()
    : camera_(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, 800.0f / 600.0f, 0.1f, 100.0f) // Initialize camera here
{
    glm::vec3 front = glm::normalize(camera_.target - camera_.position);
    yaw_ = glm::degrees(atan2(front.z, front.x));
    pitch_ = glm::degrees(asin(front.y));

    std::cout << "Application created." << std::endl;
}

Application::~Application()
{
    shutdown(); // Call explicit shutdown (optional if RAII is sufficient)
    std::cout << "Application destroyed." << std::endl;
}

bool Application::initialize()
{
    std::cout << "Initializing Application..." << std::endl;
    if (!initWindow())
        return false;
    if (!initOpenGL())
        return false; // Initialize GL state *after* context is current

    setupScene(); // Populate world etc.

    if (!loadResources())
        return false;
    std::cout << "Application initialized successfully." << std::endl;
    return true;
}

void Application::run()
{
    if (!window_)
    {
        std::cerr << "Cannot run application without a window." << std::endl;
        return;
    }

    std::cout << "Starting main loop..." << std::endl;
    // Simple delta time calculation
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    // Reset FPS counters when starting the loop (just in case run() is called multiple times)
    frameCount_ = 0;
    double totalTime_ = 0.0;
    double timeSinceLastPrint_ = 0.0;

    double t = 0.0;
    // Fixed physics delta time step for simulation consistency
    double dt = 0.01;

    double accumulator = 0.0;

    while (!window_->shouldClose())
    {
        // Calculate delta time
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = currentFrameTime - lastFrameTime;
        double deltaTime = diff.count();

        // Guard against simulation death spiral if deltaTime gets too big
        if (deltaTime > 0.25)
            deltaTime = 0.25;
        lastFrameTime = currentFrameTime;

        accumulator += deltaTime;

        frameCount_++;                    // Increment frame count
        totalTime_ += deltaTime;          // Add delta time to total
        timeSinceLastPrint_ += deltaTime; // Add delta time to time since last print

        // Check if 5 seconds have passed
        if (timeSinceLastPrint_ >= 5.0f)
        {
            // Calculate average FPS over the accumulated time
            double averageFPS = (double)frameCount_ / totalTime_; // Ensure float division

            // Print the result
            std::cout << "Average FPS: " << averageFPS << std::endl;

            // Reset the counters for the next 5-second interval
            frameCount_ = 0;
            totalTime_ = 0.0;
            timeSinceLastPrint_ = 0.0; // Reset the timer
        }

        // Update the camera projeciton in case user resizes window, not am defensively setting glViewport again here (is done in callback in Window too)
        int fbW, fbH;
        glfwGetFramebufferSize(window_->getGLFWwindow(), &fbW, &fbH);
        glViewport(0, 0, fbW, fbH);
        camera_.aspectRatio = float(fbW) / float(fbH);

        // 1. Input
        processInput();

        // 1.5 Treat mouse look for camera as real time subsystem, update every frame
        yaw_ += input_.mouseDX * mouseSens_;
        pitch_ += input_.mouseDY * mouseSens_;
        // constrain pitch
        pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);

        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front = glm::normalize(front);

        camera_.target = camera_.position + front;

        // reset mouse deltas for next frame
        input_.mouseDX = input_.mouseDY = 0.0f;

        // 2. Update Game Logic (fixed physics step consuming the fime "created" by frame)
        while (accumulator >= dt)
        {
            // previousState = currentState;
            // integrate(currentState, t, dt);
            update(static_cast<float>(dt));
            t += dt;
            accumulator -= dt;
        }

        // EXAMPLE later we can interpolate the render to get the fractional physics step we could not perform in simulation
        // const double alpha = accumulator / dt;
        // State state = currentState * alpha +
        //     previousState * ( 1.0 - alpha );
        // render( state );

        // 3. Render
        render();

        // 4. Swap Buffers and Poll Events
        window_->swapBuffers();
        window_->pollEvents();
    }
    std::cout << "Exiting main loop." << std::endl;
}

bool Application::initWindow()
{
    try
    {
        // Use smart pointer for automatic memory management
        window_ = std::make_unique<Window>(800, 600, "OpenGL Cube World - Refactored", kVSyncEnabled);
        // Window constructor handles GLFW init, window creation, context, and GLAD (if used)
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Window Initialization Error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool Application::initOpenGL()
{
    // Ensure this is called *after* the OpenGL context is created and made current

    // --- Current State ---
    glEnable(GL_DEPTH_TEST); // Keep depth testing enabled

    // --- ADD THESE LINES ---
    glEnable(GL_CULL_FACE); // Enable face culling
    glCullFace(GL_BACK);    // Tell OpenGL to cull back-facing triangles
    glFrontFace(GL_CCW);    // Define CCW winding order as front-facing (this is the default and matches your MeshBuilder)
    // ---------------------

    // Set clear color (can also be done per-frame in render)
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    std::cout << "OpenGL state initialized (Depth Test & Back-Face Culling enabled)." << std::endl; // Updated message
    return true;                                                                                    // Add error checking if needed
}

bool Application::loadResources()
{
    std::cout << "Loading resources..." << std::endl;
    // Load Shaders
    try
    {
        blockShader_ = std::make_unique<Shader>("assets/shaders/shader.vs", "assets/shaders/shader.fs");
        if (blockShader_->ID == 0)
        {
            throw std::runtime_error("Shader compilation/linking failed.");
        }
        std::cout << "Shader loaded successfully." << std::endl;
    }
    catch (const std::exception &e)
    { // Catch potential file read errors too
        std::cerr << "Shader Loading Error: " << e.what() << std::endl;
        return false;
    }

    // Load Texture

    int dirt = 0;
    int stone = 1;
    int sand = 2;
    int grass_top = 3;
    int grass_side = 4;
    int wood_oak_top = 5;
    int wood_oak_side = 6;
    int cobblestone = 7;
    int oak_plank = 8;
    int oak_leaf = 9;

    layer_mapping.emplace(BlockType::DIRT, FaceToLayer{dirt, dirt, dirt, dirt, dirt, dirt});
    layer_mapping.emplace(BlockType::STONE, FaceToLayer{stone, stone, stone, stone, stone, stone});
    layer_mapping.emplace(BlockType::SAND, FaceToLayer{sand, sand, sand, sand, sand, sand});
    layer_mapping.emplace(BlockType::GRASS, FaceToLayer{grass_side, grass_side, grass_top, dirt, grass_side, grass_side});
    layer_mapping.emplace(BlockType::WOOD_OAK, FaceToLayer{wood_oak_side, wood_oak_side, wood_oak_top, wood_oak_top, wood_oak_side, wood_oak_side});
    layer_mapping.emplace(BlockType::COBBLESTONE, FaceToLayer{cobblestone, cobblestone, cobblestone, cobblestone, cobblestone, cobblestone});
    layer_mapping.emplace(BlockType::OAK_PLANK, FaceToLayer{oak_plank, oak_plank, oak_plank, oak_plank, oak_plank, oak_plank});
    layer_mapping.emplace(BlockType::OAK_LEAF, FaceToLayer{oak_leaf, oak_leaf, oak_leaf, oak_leaf, oak_leaf, oak_leaf});

    glGenTextures(1, &blockTextureArrayId);
    glBindTexture(GL_TEXTURE_2D_ARRAY, blockTextureArrayId);

    int textureWidth = 16;  // Example
    int textureHeight = 16; // Example
    int layerCount = 10;    // Example: dirt, stone, grass_top, grass_side
    int mipLevelCount = 4;  // Calculate based on size, e.g., log2(max(width, height)) + 1

    glTexImage3D(GL_TEXTURE_2D_ARRAY,
                 0,                // Base mipmap level for initial definition©
                 GL_RGB8,          // Internal format (requesting 8 bits per channel RGB)
                 textureWidth,     // Width
                 textureHeight,    // Height
                 layerCount,       // Depth (number of layers)
                 0,                // Border (must be 0 in core profile)
                 GL_RGB,           // Format of pixel data (doesn't matter when data is NULL)
                 GL_UNSIGNED_BYTE, // Type of pixel data (doesn't matter when data is NULL)
                 NULL);            // Pass NULL data pointer to only allocate storage

    stbi_set_flip_vertically_on_load(true);

    int x, y, n;
    unsigned char *data = stbi_load("assets/textures/dirt_16x16.png", &x, &y, &n, 0);
    std::cout << "stb image x: " << x << " y: " << y << " n: " << n << '\n';
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,                // Mipmap level (0 is base)
                    0, 0,             // xoffset, yoffset (always 0 for full layer update)
                    dirt,             // The layer index to upload to
                    textureWidth,     // Width of the data being uploaded
                    textureHeight,    // Height of the data being uploaded
                    1,                // Depth (always 1 for 2D array layers)
                    GL_RGB,           // Format of the source data (e.g., from stb_image)
                    GL_UNSIGNED_BYTE, // Type of the source data
                    data);            // Pointer to the image data
    stbi_image_free(data);

    data = stbi_load("assets/textures/stone_16x16.png", &x, &y, &n, 0);
    std::cout << "stb image x: " << x << " y: " << y << " n: " << n << '\n';
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,                // Mipmap level (0 is base)
                    0, 0,             // xoffset, yoffset (always 0 for full layer update)
                    stone,            // The layer index to upload to
                    textureWidth,     // Width of the data being uploaded
                    textureHeight,    // Height of the data being uploaded
                    1,                // Depth (always 1 for 2D array layers)
                    GL_RGB,           // Format of the source data (e.g., from stb_image)
                    GL_UNSIGNED_BYTE, // Type of the source data
                    data);            // Pointer to the image data
    stbi_image_free(data);

    data = stbi_load("assets/textures/sand_16x16.png", &x, &y, &n, 0);
    std::cout << "stb image x: " << x << " y: " << y << " n: " << n << '\n';
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,                // Mipmap level (0 is base)
                    0, 0,             // xoffset, yoffset (always 0 for full layer update)
                    sand,             // The layer index to upload to
                    textureWidth,     // Width of the data being uploaded
                    textureHeight,    // Height of the data being uploaded
                    1,                // Depth (always 1 for 2D array layers)
                    GL_RGB,           // Format of the source data (e.g., from stb_image)
                    GL_UNSIGNED_BYTE, // Type of the source data
                    data);            // Pointer to the image data
    stbi_image_free(data);

    data = stbi_load("assets/textures/grass_top_16x16.png", &x, &y, &n, 0);
    std::cout << "stb image x: " << x << " y: " << y << " n: " << n << '\n';
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,                // Mipmap level (0 is base)
                    0, 0,             // xoffset, yoffset (always 0 for full layer update)
                    grass_top,        // The layer index to upload to
                    textureWidth,     // Width of the data being uploaded
                    textureHeight,    // Height of the data being uploaded
                    1,                // Depth (always 1 for 2D array layers)
                    GL_RGB,           // Format of the source data (e.g., from stb_image)
                    GL_UNSIGNED_BYTE, // Type of the source data
                    data);            // Pointer to the image data
    stbi_image_free(data);

    data = stbi_load("assets/textures/grass_side_16x16.png", &x, &y, &n, 0);
    std::cout << "stb image x: " << x << " y: " << y << " n: " << n << '\n';
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,                // Mipmap level (0 is base)
                    0, 0,             // xoffset, yoffset (always 0 for full layer update)
                    grass_side,       // The layer index to upload to
                    textureWidth,     // Width of the data being uploaded
                    textureHeight,    // Height of the data being uploaded
                    1,                // Depth (always 1 for 2D array layers)
                    GL_RGB,           // Format of the source data (e.g., from stb_image)
                    GL_UNSIGNED_BYTE, // Type of the source data
                    data);            // Pointer to the image data
    stbi_image_free(data);

    data = stbi_load("assets/textures/oak_top_16x16.png", &x, &y, &n, 0);
    std::cout << "stb image x: " << x << " y: " << y << " n: " << n << '\n';
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,                // Mipmap level (0 is base)
                    0, 0,             // xoffset, yoffset (always 0 for full layer update)
                    wood_oak_top,     // The layer index to upload to
                    textureWidth,     // Width of the data being uploaded
                    textureHeight,    // Height of the data being uploaded
                    1,                // Depth (always 1 for 2D array layers)
                    GL_RGB,           // Format of the source data (e.g., from stb_image)
                    GL_UNSIGNED_BYTE, // Type of the source data
                    data);            // Pointer to the image data
    stbi_image_free(data);

    data = stbi_load("assets/textures/oak_16x16.png", &x, &y, &n, 0);
    std::cout << "stb image x: " << x << " y: " << y << " n: " << n << '\n';
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,                // Mipmap level (0 is base)
                    0, 0,             // xoffset, yoffset (always 0 for full layer update)
                    wood_oak_side,    // The layer index to upload to
                    textureWidth,     // Width of the data being uploaded
                    textureHeight,    // Height of the data being uploaded
                    1,                // Depth (always 1 for 2D array layers)
                    GL_RGB,           // Format of the source data (e.g., from stb_image)
                    GL_UNSIGNED_BYTE, // Type of the source data
                    data);            // Pointer to the image data
    stbi_image_free(data);

    data = stbi_load("assets/textures/cobblestone_16x16.png", &x, &y, &n, 0);
    std::cout << "stb image x: " << x << " y: " << y << " n: " << n << '\n';
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,                // Mipmap level (0 is base)
                    0, 0,             // xoffset, yoffset (always 0 for full layer update)
                    cobblestone,      // The layer index to upload to
                    textureWidth,     // Width of the data being uploaded
                    textureHeight,    // Height of the data being uploaded
                    1,                // Depth (always 1 for 2D array layers)
                    GL_RGB,           // Format of the source data (e.g., from stb_image)
                    GL_UNSIGNED_BYTE, // Type of the source data
                    data);            // Pointer to the image data
    stbi_image_free(data);

    data = stbi_load("assets/textures/oak_plank_16x16.png", &x, &y, &n, 0);
    std::cout << "stb image x: " << x << " y: " << y << " n: " << n << '\n';
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,                // Mipmap level (0 is base)
                    0, 0,             // xoffset, yoffset (always 0 for full layer update)
                    oak_plank,        // The layer index to upload to
                    textureWidth,     // Width of the data being uploaded
                    textureHeight,    // Height of the data being uploaded
                    1,                // Depth (always 1 for 2D array layers)
                    GL_RGB,           // Format of the source data (e.g., from stb_image)
                    GL_UNSIGNED_BYTE, // Type of the source data
                    data);            // Pointer to the image data
    stbi_image_free(data);

    data = stbi_load("assets/textures/oak_leaf_16x16.png", &x, &y, &n, 0);
    std::cout << "stb image x: " << x << " y: " << y << " n: " << n << '\n';
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,                // Mipmap level (0 is base)
                    0, 0,             // xoffset, yoffset (always 0 for full layer update)
                    oak_leaf,         // The layer index to upload to
                    textureWidth,     // Width of the data being uploaded
                    textureHeight,    // Height of the data being uploaded
                    1,                // Depth (always 1 for 2D array layers)
                    GL_RGB,           // Format of the source data (e.g., from stb_image)
                    GL_UNSIGNED_BYTE, // Type of the source data
                    data);            // Pointer to the image data
    std::cout << "about to free\n";
    stbi_image_free(data);

    std::cout << "finished loading images\n";

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    // Set wrapping parameters for S (horizontal) and T (vertical) texture coordinates
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT); // Repeat the texture horizontally
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT); // Repeat the texture vertically

    // Set filtering parameters for magnification (zooming in) and minification (zooming out)
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Use linear interpolation for magnification
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    // Create Mesh
    MeshData worldMeshData;

    std::cout << "about to generate world mesh\n";

    MeshBuilder::generateWorldMesh(gameWorld_, worldMeshData, layer_mapping);

    // *** ADD THIS CHECK ***
    std::cout << "MeshData sizes before interleaving:\n";
    std::cout << "  Vertices:     " << worldMeshData.vertices.size() << "\n";
    std::cout << "  Normals:      " << worldMeshData.normals.size() << "\n";
    std::cout << "  TexCoords:    " << worldMeshData.texCoords.size() << "\n";
    std::cout << "  LayerIndices: " << worldMeshData.layerIndices.size() << "\n";
    std::cout << "  Indices:      " << worldMeshData.indices.size() << "\n";
    assert(worldMeshData.vertices.size() == worldMeshData.normals.size());
    assert(worldMeshData.vertices.size() == worldMeshData.texCoords.size());
    assert(worldMeshData.vertices.size() == worldMeshData.layerIndices.size());
    // *** END ADDED CHECK ***

    std::vector<float> vertices = worldMeshData.getInterleavedVertices();

    try
    {
        worldMesh_ = std::make_unique<Mesh>(vertices.data(),
                                            vertices.size() * sizeof(float),
                                            worldMeshData.indices.data(),
                                            worldMeshData.indices.size() * sizeof(unsigned int),
                                            worldMeshData.getVertexStride(),
                                            worldMeshData.attributeLayout);
        if (worldMesh_->VAO == 0)
        { // Check if VAO creation failed (though Mesh constructor doesn't explicitly return status)
            throw std::runtime_error("Mesh VAO creation failed (or Mesh constructor indicated error).");
        }
        std::cout << "Cube mesh created successfully." << std::endl;
    }
    catch (const std::exception &e)
    { // Catch potential errors if Mesh throws
        std::cerr << "Mesh Creation Error: " << e.what() << std::endl;
        // Clean up already loaded shader if mesh fails
        blockShader_.reset(); // Release shader ownership
        return false;
    }

    // Create Renderer (after mesh and shader are ready)
    // Renderer constructor takes references, so ensure Mesh and Shader exist
    renderer_ = std::make_unique<Renderer>(*worldMesh_, *blockShader_);
    std::cout << "Renderer created." << std::endl;

    return true;
}

void Application::setupScene()
{

    std::cout << "Setting up scene..." << std::endl;

    for (int z = 0; z < 16; ++z)
    {
        for (int x = 0; x < 16; ++x)
        {
            gameWorld_.addBlock(x, 0, z, BlockType::DIRT);
        }
    }

    gameWorld_.addBlock(3, 1, 3, BlockType::WOOD_OAK);
    gameWorld_.addBlock(3, 2, 3, BlockType::WOOD_OAK);
    gameWorld_.addBlock(3, 3, 3, BlockType::WOOD_OAK);

    gameWorld_.addBlock(4, 3, 3, BlockType::OAK_LEAF);

    gameWorld_.addBlock(1, 1, 3, BlockType::SAND);

    gameWorld_.addBlock(3, 1, 1, BlockType::COBBLESTONE);

    gameWorld_.addBlock(5, 1, 5, BlockType::GRASS);

    gameWorld_.addBlock(7, 1, 7, BlockType::OAK_PLANK);

    gameWorld_.addBlock(7, 1, 5, BlockType::STONE);

    std::cout
        << "Scene setup complete. "
        << std::endl;
}

void Application::processInput()
{
    GLFWwindow *w = window_->getGLFWwindow();

    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(w, true);
        return;
    }

    // -- keyboard --
    input_.forward = (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS);
    input_.backward = (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS);
    input_.left = (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS);
    input_.right = (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS);
    input_.up = (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS);
    input_.down = (glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

    // -- mouse --
    double xpos, ypos;
    glfwGetCursorPos(w, &xpos, &ypos);
    if (firstMouse_)
    {
        lastX_ = xpos;
        lastY_ = ypos;
        firstMouse_ = false;
    }
    input_.mouseDX = float(xpos - lastX_);
    input_.mouseDY = float(lastY_ - ypos); // reversed: y goes down→up
    lastX_ = xpos;
    lastY_ = ypos;
}

void Application::update(float dt)
{
    // --- keyboard move ---
    glm::vec3 forwardDir = glm::normalize(camera_.target - camera_.position);
    glm::vec3 rightDir = glm::normalize(glm::cross(forwardDir, camera_.up));

    glm::vec3 motion(0.0f);
    if (input_.forward)
        motion += forwardDir;
    if (input_.backward)
        motion -= forwardDir;
    if (input_.right)
        motion += rightDir;
    if (input_.left)
        motion -= rightDir;
    if (input_.up)
        motion += camera_.up;
    if (input_.down)
        motion -= camera_.up;

    if (glm::length(motion) > 0.0f)
    {
        motion = glm::normalize(motion) * cameraSpeed_ * dt;
        camera_.position += motion;
        camera_.target += motion;
    }
}

void Application::render()
{
    // Renderer already handles clear, shader use, matrix setup, drawing
    if (renderer_)
    {
        renderer_->render(gameWorld_, camera_, blockTextureArrayId);
    }
}

void Application::shutdown()
{
    std::cout << "Shutting down Application..." << std::endl;
    // Cleanup is largely handled by unique_ptr destructors calling the
    // destructors of Window, Shader, Mesh, Renderer in the correct order.
    // Explicit cleanup can be done here if needed (e.g., detaching shaders before deleting program if not done in Shader destructor)
    renderer_.reset();
    worldMesh_.reset();
    blockShader_.reset();
    glDeleteTextures(1, &blockTextureArrayId);
    window_.reset(); // This triggers Window destructor, cleaning up GLFW
    std::cout << "Application shutdown complete." << std::endl;
}