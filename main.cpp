#include <GLFW/glfw3.h> // Include the GLFW header
#include <iostream>     // For console output (errors)

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // --- Configure the Window and OpenGL Context ---
    // Set the required OpenGL version (e.g., 3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Use the core profile (modern OpenGL)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Enable forward compatibility (required on macOS)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // Make the window non-resizable (optional)
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


    // --- Create a Windowed Mode Window and its OpenGL Context ---
    // Arguments: width, height, title, monitor (for fullscreen), share (for sharing resources)
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Cube", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // Clean up GLFW before exiting
        return -1;
    }

    // --- Make the Window's Context the Current One ---
    // All subsequent OpenGL commands will be drawn to this window's context
    glfwMakeContextCurrent(window);

    // --- Optional: Set the Viewport ---
    // This maps the normalized device coordinates (-1 to 1) to the window coordinates
    // In this simple case, it's usually set to the window size.
    // For now, GLFW handles the initial viewport, but you'll likely set this
    // and handle window resizing later.
    // glViewport(0, 0, 800, 600);


    // --- Main Rendering Loop ---
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // --- Input Handling ---
        // Check for and process events (keyboard, mouse, etc.)
        glfwPollEvents();

        // --- Rendering Commands Go Here ---
        // For now, just clear the screen to a color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set the clear color (dark cyan)
        glClear(GL_COLOR_BUFFER_BIT);        // Clear the color buffer

        // --- Swap Buffers ---
        // Swap the front and back buffers. The back buffer is where we draw,
        // the front buffer is what's currently displayed. Swapping makes the
        // newly drawn frame visible.
        glfwSwapBuffers(window);
    }

    // --- Clean Up ---
    // Destroy the window and its context
    glfwDestroyWindow(window);
    // Terminate GLFW
    glfwTerminate();

    return 0;
}
