#ifndef WINDOW_H
#define WINDOW_H

#include <string>

// Forward declare GLFWwindow to avoid including glfw3.h in the header
struct GLFWwindow;

class Window
{
public:
    // Constructor initializes GLFW and creates the window
    Window(int width, int height, const std::string &title);
    // Destructor cleans up GLFW resources
    ~Window();

    // Prevent copying/assignment
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    // Check if the window should close (e.g., user clicked the close button)
    bool shouldClose() const;

    // Process pending window events (input, resize, etc.)
    void pollEvents() const;

    // Swap the front and back buffers to display the rendered frame
    void swapBuffers() const;

    // Make the OpenGL context of this window current on the calling thread
    void makeContextCurrent() const;

    // Getters (optional, but can be useful)
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    GLFWwindow *getGLFWwindow() const { return glfwWindow_; } // Provide access if needed externally

    // Static callback function for framebuffer resizing
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

private:
    GLFWwindow *glfwWindow_ = nullptr;
    int width_;
    int height_;
    std::string title_;

    bool initializeGLFW();
    bool createWindow();
    void setupCallbacks();
};

#endif // WINDOW_H