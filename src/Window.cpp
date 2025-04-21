#include "Window.h"
#include <OpenGL/gl.h>
#include <cstddef>
#include <stdexcept> // For runtime_error
#include <iostream>  // For cerr

// Include GLFW *only* in the .cpp file
#include <GLFW/glfw3.h>
#include <string>

#ifdef __APPLE__
#include <OpenGL/gl3.h> // Or Glad/GLEW if you switch
#endif

Window::Window(int width, int height, const std::string &title, bool isVSyncEnabled)
    : width_(width), height_(height), title_(title), isVSyncEnabled_(isVSyncEnabled)
{
    if (!initializeGLFW())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    if (!createWindow())
    {
        glfwTerminate(); // Clean up GLFW if window creation fails
        throw std::runtime_error("Failed to create GLFW window");
    }
    // Store 'this' pointer to retrieve in static callbacks
    glfwSetWindowUserPointer(glfwWindow_, this);

    setupCallbacks();
    makeContextCurrent(); // Make context current immediately after creation
    setVSyncEnabled(isVSyncEnabled_);

    glfwSetInputMode(glfwWindow_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // --- IMPORTANT: Initialize OpenGL function pointers (if using Glad/GLEW) ---
    // If you were using Glad:
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //     glfwDestroyWindow(glfwWindow_);
    //     glfwTerminate();
    //     throw std::runtime_error("Failed to initialize GLAD");
    // }
    // On macOS with the framework, this step is usually implicit after context creation.
    std::cout
        << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
}

Window::~Window()
{
    if (glfwWindow_)
    {
        glfwDestroyWindow(glfwWindow_);
    }
    glfwTerminate(); // Terminate GLFW when the last window is destroyed
    std::cout << "Window and GLFW cleaned up." << std::endl;
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(glfwWindow_);
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(glfwWindow_);
}

void Window::makeContextCurrent() const
{
    glfwMakeContextCurrent(glfwWindow_);

    if (isVSyncEnabled_)
    {
        std::cout << "Setting VSync enabled" << std::endl;
        glfwSwapInterval(0);
    }
    else
    {
        std::cout << "Setting VSync disabled" << std::endl;
        glfwSwapInterval(1);
    }
}

void Window::setVSyncEnabled(bool enabled)
{
    // Ensure this window’s context is current
    if (glfwGetCurrentContext() != glfwWindow_)
    {
        throw std::runtime_error(
            "Window::setVSyncEnabled requires this window's context to be current");
    }

    isVSyncEnabled_ = enabled;
    // 1 = enable VSync, 0 = disable
    glfwSwapInterval(enabled ? 1 : 0);

    std::cout << "VSync " << (enabled ? "enabled" : "disabled") << std::endl;
}

// Static callback implementation
void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // Make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (self)
    {
        self->width_ = width;
        self->height_ = height;
    }
}

bool Window::initializeGLFW()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    // Set GLFW window hints (OpenGL version, profile, etc.)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#endif
    return true;
}

bool Window::createWindow()
{
    glfwWindow_ = glfwCreateWindow(width_, height_, title_.c_str(), NULL, NULL);
    if (glfwWindow_ == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return false;
    }
    return true;
}

void Window::setupCallbacks()
{
    // Set the framebuffer size callback
    glfwSetFramebufferSizeCallback(glfwWindow_, framebuffer_size_callback);
    // Add other callbacks here (keyboard, mouse, etc.) as needed
}
