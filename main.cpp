#include <GLFW/glfw3.h> // Include the GLFW header
#include <iostream>     // For console output (errors)
#include <fstream>      // For file reading
#include <sstream>      // For string streams
#include <string>       // For using strings
#include <vector>       // For storing block positions
#include <cmath>        // For sin/cos in animation

// We'll need to include the core OpenGL header specific to macOS
// after making the context current.
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// --- Helper Functions (Could be private methods of Shader/Renderer classes) ---

// Helper function to read shader source code from a file
std::string readShaderFile(const std::string& filePath) {
    std::ifstream shaderFile;
    std::stringstream shaderStream;
    std::string shaderCode;

    // Ensure ifstream objects can throw exceptions
    shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Open file
        shaderFile.open(filePath);
        // Read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();
        // Close file handlers
        shaderFile.close();
        // Convert stream into string
        shaderCode = shaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << std::endl;
        return ""; // Return empty string on failure
    }
    return shaderCode;
}


// Helper function to compile shaders
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type); // Create a shader object (ID)
    glShaderSource(id, 1, &source, NULL); // Set the shader source code
    glCompileShader(id); // Compile the shader

    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        if (type == GL_VERTEX_SHADER) std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n";
        else if (type == GL_FRAGMENT_SHADER) std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n";
        else std::cerr << "ERROR::SHADER::UNKNOWN_TYPE::COMPILATION_FAILED\n";
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cerr << infoLog << std::endl;
        glDeleteShader(id); // Delete the shader if compilation failed
        return 0;
    }
    return id; // Return the shader ID
}


// --- Class Definitions (Interfaces and minimal implementations) ---
// In a real project, these would be in separate .h and .cpp files.

// Represents a compiled and linked shader program
class Shader {
public:
    unsigned int ID; // The OpenGL shader program ID

    // Constructor reads, compiles, and links shaders
    Shader(const std::string& vertexPath, const std::string& fragmentPath) {
        // Read shader files
        std::string vertexCode = readShaderFile(vertexPath);
        std::string fragmentCode = readShaderFile(fragmentPath);

        if (vertexCode.empty() || fragmentCode.empty()) {
            ID = 0; // Indicate failure
            return;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        // Compile shaders
        unsigned int vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
        unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

        if (vertex == 0 || fragment == 0) {
            ID = 0; // Indicate failure
            return;
        }

        // Link program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);

        // Check for linking errors
        int success;
        char infoLog[512];
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            glDeleteProgram(ID);
            ID = 0; // Indicate failure
        }

        // Delete the shaders as they're linked into our program now
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // Destructor to clean up the shader program
    ~Shader() {
        if (ID != 0) {
            glDeleteProgram(ID);
        }
    }

    // Use the shader program
    void use() const {
        glUseProgram(ID);
    }

    // Utility uniform functions
    void setMatrix4(const std::string& name, const glm::mat4& matrix) const {
        // Get the location of the uniform variable in the shader
        // We get the location every time here for simplicity, but for performance
        // you would typically get all uniform locations once after linking.
        int uniformLocation = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    // Add other uniform setters as needed (setVec3, setFloat, setInt, etc.)
};

// Represents geometric data (vertices, indices) and its OpenGL buffers (VAO, VBO, EBO)
class Mesh {
public:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount; // Number of indices to draw

    // Constructor takes vertex and index data and sets up OpenGL buffers
    Mesh(const float* vertices, size_t vertexSize, const unsigned int* indices, size_t indexSize, size_t vertexStride, const std::vector<std::pair<unsigned int, size_t>>& attributeLayout) {
        indexCount = indexSize / sizeof(unsigned int);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices, GL_STATIC_DRAW);

        // Configure vertex attributes based on the provided layout
        for (const auto& attr : attributeLayout) {
            unsigned int location = attr.first;
            size_t offset = attr.second;
             // Assuming size=3, type=GL_FLOAT, normalized=GL_FALSE for this cube example
            glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, vertexStride, (void*)offset);
            glEnableVertexAttribArray(location);
        }

        glBindVertexArray(0); // Unbind VAO
        // Note: VBO and EBO are unbound when VAO is unbound
    }

    // Destructor to clean up OpenGL buffers
    ~Mesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    // Bind the mesh's VAO for drawing
    void bind() const {
        glBindVertexArray(VAO);
    }

    // Unbind the mesh's VAO
    void unbind() const {
        glBindVertexArray(0);
    }
};

// Represents the camera's view and projection
class Camera {
public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    Camera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 upVec, float fovDeg, float aspect, float near, float far)
        : position(pos), target(lookAt), up(upVec), fov(fovDeg), aspectRatio(aspect), nearPlane(near), farPlane(far) {}

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 getProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }
};

// Simple representation of the game world holding block positions
class World {
public:
    std::vector<glm::vec3> blockPositions;

    void addBlock(const glm::vec3& pos) {
        blockPositions.push_back(pos);
    }

    const std::vector<glm::vec3>& getBlocksToRender() const {
        // In a real game, this would return visible blocks (e.g., based on camera frustum, chunks)
        return blockPositions;
    }
};

// Handles the rendering process
class Renderer {
private:
    const Mesh& meshToDraw; // Reference to the shared mesh (e.g., cube mesh)
    const Shader& shaderToUse; // Reference to the shared shader

public:
    Renderer(const Mesh& mesh, const Shader& shader)
        : meshToDraw(mesh), shaderToUse(shader) {
        // Renderer constructor can set up global GL state if needed
        glEnable(GL_DEPTH_TEST);
    }

    void render(const World& world, const Camera& camera) {
        // Clear buffers
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        shaderToUse.use();

        // Set view and projection matrices (these are usually per-frame, not per-object)
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        shaderToUse.setMatrix4("view", view);
        shaderToUse.setMatrix4("projection", projection);

        // Bind the mesh (can be done once if all objects use the same mesh)
        meshToDraw.bind();

        // Iterate through objects in the world and draw them
        const auto& blocks = world.getBlocksToRender();
        for (const auto& blockPos : blocks) {
            // Calculate the model matrix for this specific block
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, blockPos); // Move the block to its world position
            // Add rotation or scale here if blocks could be rotated/scaled individually

            // Pass the model matrix to the shader
            shaderToUse.setMatrix4("model", model);

            // Draw the mesh using the bound VAO and active shader
            glDrawElements(GL_TRIANGLES, meshToDraw.indexCount, GL_UNSIGNED_INT, 0);
        }

        meshToDraw.unbind(); // Unbind mesh after drawing
        glUseProgram(0); // Unbind shader after drawing
    }
};


// --- Main Cube Data (Moved outside main function) ---
float cubeVertices[] = {
    // Positions           // Colors
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Back bottom left (Red)
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Back bottom right (Red)
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Back top right (Red)
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Back top left (Red)

    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Front bottom left (Green)
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Front bottom right (Green)
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Front top right (Green)
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Front top left (Green)
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
    6, 7, 3
};


int main() {
    // --- 1. Initialize Systems (GLFW, Window, OpenGL Context) ---
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Cube World", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // *** IMPORTANT for macOS ***
#ifdef __APPLE__
    // Include OpenGL headers after context is current
#endif

    // --- 2. Create Rendering Resources (Mesh, Shader) ---

    // Define the layout of the vertex data for the Mesh constructor
    // Location 0: Position (3 floats), offset 0
    // Location 1: Color (3 floats), offset 3 * sizeof(float)
    std::vector<std::pair<unsigned int, size_t>> cubeAttributeLayout = {
        {0, 0}, // Position attribute at location 0, offset 0
        {1, 3 * sizeof(float)} // Color attribute at location 1, offset 3 floats
    };
    size_t cubeVertexStride = 6 * sizeof(float); // Total size of one vertex (3 pos + 3 color)

    // Create the cube mesh object
    Mesh cubeMesh(cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices), cubeVertexStride, cubeAttributeLayout);
    if (cubeMesh.VAO == 0) {
        // Handle mesh creation failure
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Create the shader program object
    Shader blockShader("shader.vs", "shader.fs");
    if (blockShader.ID == 0) {
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

    while (!glfwWindowShouldClose(window)) {
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
