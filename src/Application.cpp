#include "Application.h"
#include "Window.h" // Need full definition now
#include "Shader.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Camera.h" // Include Camera.h
#include "World.h"  // Include World.h
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

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

namespace
{
    constexpr bool kVSyncEnabled = false;
}

// --- Cube Data (Moved from main.cpp) ---
// Could be in a separate file or class later
namespace CubeData
{
    // 24 vertices: 4 per face × 6 faces
    // Each vertex: position (3), color (3), normal (3) = 9 floats
    const float vertices[] = {
        // Back face (normal = 0,0,-1)
        -0.5f,
        -0.5f,
        -0.5f,
        1,
        0,
        0,
        0,
        0,
        -1,
        0.5f,
        -0.5f,
        -0.5f,
        1,
        0,
        0,
        0,
        0,
        -1,
        0.5f,
        0.5f,
        -0.5f,
        1,
        0,
        0,
        0,
        0,
        -1,
        -0.5f,
        0.5f,
        -0.5f,
        1,
        0,
        0,
        0,
        0,
        -1,

        // Front face (normal = 0,0,1)
        -0.5f,
        -0.5f,
        0.5f,
        0,
        1,
        0,
        0,
        0,
        1,
        0.5f,
        -0.5f,
        0.5f,
        0,
        1,
        0,
        0,
        0,
        1,
        0.5f,
        0.5f,
        0.5f,
        0,
        1,
        0,
        0,
        0,
        1,
        -0.5f,
        0.5f,
        0.5f,
        0,
        1,
        0,
        0,
        0,
        1,

        // Left face (normal = -1,0,0)
        -0.5f,
        -0.5f,
        -0.5f,
        0,
        0,
        1,
        -1,
        0,
        0,
        -0.5f,
        0.5f,
        -0.5f,
        0,
        0,
        1,
        -1,
        0,
        0,
        -0.5f,
        0.5f,
        0.5f,
        0,
        0,
        1,
        -1,
        0,
        0,
        -0.5f,
        -0.5f,
        0.5f,
        0,
        0,
        1,
        -1,
        0,
        0,

        // Right face (normal = 1,0,0)
        0.5f,
        -0.5f,
        -0.5f,
        1,
        1,
        0,
        1,
        0,
        0,
        0.5f,
        -0.5f,
        0.5f,
        1,
        1,
        0,
        1,
        0,
        0,
        0.5f,
        0.5f,
        0.5f,
        1,
        1,
        0,
        1,
        0,
        0,
        0.5f,
        0.5f,
        -0.5f,
        1,
        1,
        0,
        1,
        0,
        0,

        // Bottom face (normal = 0,-1,0)
        -0.5f,
        -0.5f,
        -0.5f,
        1,
        0,
        1,
        0,
        -1,
        0,
        -0.5f,
        -0.5f,
        0.5f,
        1,
        0,
        1,
        0,
        -1,
        0,
        0.5f,
        -0.5f,
        0.5f,
        1,
        0,
        1,
        0,
        -1,
        0,
        0.5f,
        -0.5f,
        -0.5f,
        1,
        0,
        1,
        0,
        -1,
        0,

        // Top face (normal = 0,1,0)
        -0.5f,
        0.5f,
        -0.5f,
        0,
        1,
        1,
        0,
        1,
        0,
        0.5f,
        0.5f,
        -0.5f,
        0,
        1,
        1,
        0,
        1,
        0,
        0.5f,
        0.5f,
        0.5f,
        0,
        1,
        1,
        0,
        1,
        0,
        -0.5f,
        0.5f,
        0.5f,
        0,
        1,
        1,
        0,
        1,
        0,
    };

    // Indices: 2 triangles per face, 6 faces
    const unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,       // back
        4, 5, 6, 6, 7, 4,       // front
        8, 9, 10, 10, 11, 8,    // left
        12, 13, 14, 14, 15, 12, // right
        16, 17, 18, 18, 19, 16, // bottom
        20, 21, 22, 22, 23, 20  // top
    };

    // Now: pos(loc=0), color(loc=1), normal(loc=2)
    const std::vector<std::pair<unsigned int, size_t>> attributeLayout = {
        {0, 0},                 // vec3 position
        {1, 3 * sizeof(float)}, // vec3 color
        {2, 6 * sizeof(float)}  // vec3 normal
    };

    // Each vertex = 9 floats
    const size_t vertexStride = 9 * sizeof(float);
}
// --- End Cube Data ---

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
    if (!loadResources())
        return false;
    setupScene(); // Populate world etc.
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

    // --- Grid configuration ---
    const int gridWidth = 10;   // number of cubes along X
    const int gridDepth = 10;   // number of cubes along Z
    const float spacing = 1.0f; // distance between cube centers

    // Populate the game world with a flat grid at y = 0
    for (int x = 0; x < gridWidth; ++x)
    {
        for (int z = 0; z < gridDepth; ++z)
        {
            float xpos = (x - gridWidth * 0.5f) * spacing;
            float zpos = (z - gridDepth * 0.5f) * spacing;
            gameWorld_.addBlock(glm::vec3(xpos, 0.0f, zpos));
        }
    }

    std::cout << "Scene setup complete. "
              << (gridWidth * gridDepth) << " blocks added."
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