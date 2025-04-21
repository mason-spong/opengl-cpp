#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory> // For unique_ptr
#include "World.h"
#include "Camera.h"
// Forward declarations to avoid including heavy headers
class Window;
class Shader;
class Mesh;
class Renderer;

struct InputState
{
    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;

    float mouseDX = 0.0f; // mouse delta x
    float mouseDY = 0.0f; // mouse delta y
};

class Application
{
public:
    Application();
    ~Application();

    // Prevent copying/assignment
    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    // Initialize all systems and resources
    bool initialize();

    // Run the main application loop
    void run();

private:
    // Core components
    std::unique_ptr<Window> window_;
    std::unique_ptr<Shader> blockShader_;
    std::unique_ptr<Mesh> cubeMesh_;
    std::unique_ptr<Renderer> renderer_;
    World gameWorld_;
    Camera camera_;

    InputState input_;
    float cameraSpeed_ = 5.0f; // movement speed
    float mouseSens_ = 0.1f;   // look sensitivity
    float yaw_ = -90.0f;       // init so forward=(0,0,-1)
    float pitch_ = 0.0f;
    bool firstMouse_ = true;
    double lastX_ = 400.0; // center of 800×600 window
    double lastY_ = 300.0;

    int frameCount_ = 0;
    float totalTime_ = 0.0f;          // Accumulates delta time
    float timeSinceLastPrint_ = 0.0f; // Time since the last FPS was printed

    // Private helper methods for initialization steps
    bool initWindow();
    bool initOpenGL(); // For GL settings like depth test
    bool loadResources();
    void setupScene();

    // Main loop steps
    void processInput();          // Placeholder for input handling
    void update(float deltaTime); // Placeholder for game logic updates
    void render();

    // Cleanup (mostly handled by destructors/RAII, but can be explicit if needed)
    void shutdown();
};

#endif // APPLICATION_H