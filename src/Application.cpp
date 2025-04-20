#include "Application.h"
#include "Window.h" // Need full definition now
#include "Shader.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Camera.h" // Include Camera.h
#include "World.h"  // Include World.h

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

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

// --- Cube Data (Moved from main.cpp) ---
// Could be in a separate file or class later
namespace CubeData
{
    const float vertices[] = {
        // Positions          // Colors
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // Back bottom left (Red)
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // Back bottom right (Red)
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,   // Back top right (Red)
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // Back top left (Red)

        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // Front bottom left (Green)
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // Front bottom right (Green)
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,   // Front top right (Green)
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // Front top left (Green)
    };

    const unsigned int indices[] = {
        // Back face
        0, 1, 2, 2, 3, 0,
        // Front face
        4, 5, 6, 6, 7, 4,
        // Left face
        4, 0, 3, 3, 7, 4,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Bottom face
        0, 4, 5, 5, 1, 0,
        // Top face
        3, 2, 6, 6, 7, 3};

    const std::vector<std::pair<unsigned int, size_t>> attributeLayout = {
        {0, 0},                // Position attribute at location 0, offset 0
        {1, 3 * sizeof(float)} // Color attribute at location 1, offset 3 floats
    };

    const size_t vertexStride = 6 * sizeof(float); // 3 pos + 3 color

} // namespace CubeData
// --- End Cube Data ---

// --- Application Implementation ---

Application::Application()
    : camera_(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, 800.0f / 600.0f, 0.1f, 100.0f) // Initialize camera here
{
    // Constructor body (if needed)
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
    if (!loadResources())
        return false;
    setupScene(); // Populate world etc.
    std::cout << "Application initialized successfully." << std::endl;
    return true;
}

bool Application::initWindow()
{
    try
    {
        // Use smart pointer for automatic memory management
        window_ = std::make_unique<Window>(800, 600, "OpenGL Cube World - Refactored");
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
    // Set global OpenGL state
    // Ensure this is called *after* the OpenGL context is created and made current (in Window constructor)
    glEnable(GL_DEPTH_TEST);
    // Set clear color (can also be done per-frame in render)
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    std::cout << "OpenGL state initialized (Depth Test enabled)." << std::endl;
    return true; // Add error checking if needed
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

    // Create Mesh
    try
    {
        cubeMesh_ = std::make_unique<Mesh>(
            CubeData::vertices, sizeof(CubeData::vertices),
            CubeData::indices, sizeof(CubeData::indices),
            CubeData::vertexStride, CubeData::attributeLayout);
        if (cubeMesh_->VAO == 0)
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
    renderer_ = std::make_unique<Renderer>(*cubeMesh_, *blockShader_);
    std::cout << "Renderer created." << std::endl;

    return true;
}

void Application::setupScene()
{
    std::cout << "Setting up scene..." << std::endl;
    // Populate the game world
    gameWorld_.addBlock(glm::vec3(0.0f, 0.0f, 0.0f));
    gameWorld_.addBlock(glm::vec3(1.0f, 0.0f, 0.0f));
    gameWorld_.addBlock(glm::vec3(0.0f, 1.0f, 0.0f));
    gameWorld_.addBlock(glm::vec3(-1.0f, 0.0f, 0.0f));
    gameWorld_.addBlock(glm::vec3(0.0f, 0.0f, 1.0f));
    std::cout << "Scene setup complete. Blocks added." << std::endl;

    // Adjust camera aspect ratio based on actual window size
    // Note: Need a way for Application to know the window size,
    // maybe via window_->getWidth(), window_->getHeight() or callbacks.
    // For now, using the initial values.
    // camera_.aspectRatio = static_cast<float>(window_->getWidth()) / window_->getHeight();
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

    while (!window_->shouldClose())
    {
        // Calculate delta time
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> diff = currentFrameTime - lastFrameTime;
        float deltaTime = diff.count();
        lastFrameTime = currentFrameTime;

        // 1. Input
        processInput();

        // 2. Update Game Logic
        update(deltaTime);

        // 3. Render
        render();

        // 4. Swap Buffers and Poll Events
        window_->swapBuffers();
        window_->pollEvents();
    }
    std::cout << "Exiting main loop." << std::endl;
}

void Application::processInput()
{
    // Placeholder for input handling
    // Example: Check for Escape key press
    if (glfwGetKey(window_->getGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window_->getGLFWwindow(), true);
    }
    // Add camera movement controls, etc. here
}

void Application::update(float deltaTime)
{
    // Placeholder for game state updates
    // Example: Rotate the camera slowly around the origin
    // static float totalTime = 0.0f;
    // totalTime += deltaTime;
    // float radius = 7.0f;
    // camera_.position.x = sin(totalTime * 0.5f) * radius;
    // camera_.position.z = cos(totalTime * 0.5f) * radius;
    // camera_.target = glm::vec3(0.0f, 0.0f, 0.0f); // Look at origin
}

void Application::render()
{
    // Renderer already handles clear, shader use, matrix setup, drawing
    if (renderer_)
    {
        renderer_->render(gameWorld_, camera_);
    }
}

void Application::shutdown()
{
    std::cout << "Shutting down Application..." << std::endl;
    // Cleanup is largely handled by unique_ptr destructors calling the
    // destructors of Window, Shader, Mesh, Renderer in the correct order.
    // Explicit cleanup can be done here if needed (e.g., detaching shaders before deleting program if not done in Shader destructor)
    renderer_.reset();
    cubeMesh_.reset();
    blockShader_.reset();
    window_.reset(); // This triggers Window destructor, cleaning up GLFW
    std::cout << "Application shutdown complete." << std::endl;
}