#include <GLFW/glfw3.h> // Include the GLFW header
#include <OpenGL/gl.h>
#include <cstddef>
#include <iostream> // For console output (errors)
#include <string>   // For using strings
#include <utility>
#include <vector> // For storing block positions

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#include "Shader.h"
#include "Mesh.h"
#include "World.h"
#include "Camera.h"
#include "Renderer.h"

float cubeVertices[] = {
    // Positions           // Colors
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // Back bottom left (Red)
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // Back bottom right (Red)
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,   // Back top right (Red)
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // Back top left (Red)

    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // Front bottom left (Green)
    0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // Front bottom right (Green)
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,   // Front top right (Green)
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // Front top left (Green)
};

unsigned int cubeIndices[] = {
    // Back face
    0, 1, 2,
    2, 3, 0,
    // Front face
    4, 5, 6,
    6, 7, 4,
    // Left face
    4, 0, 3,
    3, 7, 4,
    // Right face
    1, 5, 6,
    6, 2, 1,
    // Bottom face
    0, 4, 5,
    5, 1, 0,
    // Top face
    3, 2, 6,
    6, 7, 3};

int main()
{
    // --- 1. Initialize Systems (GLFW, Window, OpenGL Context) ---
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Cube World", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // --- 2. Create Rendering Resources (Mesh, Shader) ---

    // Define the layout of the vertex data for the Mesh constructor
    // Location 0: Position (3 floats), offset 0
    // Location 1: Color (3 floats), offset 3 * sizeof(float)
    std::vector<std::pair<unsigned int, size_t>> cubeAttributeLayout = {
        {0, 0},                // Position attribute at location 0, offset 0
        {1, 3 * sizeof(float)} // Color attribute at location 1, offset 3 floats
    };
    size_t cubeVertexStride = 6 * sizeof(float); // Total size of one vertex (3 pos + 3 color)

    // Create the cube mesh object
    Mesh cubeMesh(cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices), cubeVertexStride, cubeAttributeLayout);
    if (cubeMesh.VAO == 0)
    {
        // Handle mesh creation failure
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Create the shader program object
    Shader blockShader("assets/shaders/shader.vs", "assets/shaders/shader.fs");
    if (blockShader.ID == 0)
    {
        // Handle shader creation failure
        // Mesh destructor will be called automatically when cubeMesh goes out of scope
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // --- 3. Create Game State (World, Camera) ---
    World gameWorld;
    Camera camera(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

    // --- 4. Populate Game State (Add Blocks to World) ---
    // Add a few blocks at different positions
    gameWorld.addBlock(glm::vec3(0.0f, 0.0f, 0.0f));
    gameWorld.addBlock(glm::vec3(1.0f, 0.0f, 0.0f));
    gameWorld.addBlock(glm::vec3(0.0f, 1.0f, 0.0f));
    gameWorld.addBlock(glm::vec3(-1.0f, 0.0f, 0.0f));
    gameWorld.addBlock(glm::vec3(0.0f, 0.0f, 1.0f));

    // --- 5. Main Render Loop ---
    Renderer mainRenderer(cubeMesh, blockShader); // Create the renderer instance

    while (!glfwWindowShouldClose(window))
    {
        // --- Input Handling (Placeholder) ---
        glfwPollEvents();
        // Add keyboard/mouse input processing here, e.g., to move the camera

        // --- Game Logic Update (Placeholder) ---
        // Update world state, block positions, etc.

        // --- Rendering ---
        // The renderer handles clearing, using shader, setting matrices, binding mesh, and drawing blocks
        mainRenderer.render(gameWorld, camera);

        // --- Swap Buffers ---
        glfwSwapBuffers(window);
    }

    // --- 6. Clean Up Systems ---
    // Class destructors (Mesh, Shader, Renderer) will automatically clean up OpenGL resources.
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
