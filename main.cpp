#include <GLFW/glfw3.h> // Include the GLFW header
#include <iostream>     // For console output (errors)
#include <cmath>        // For sin/cos in animation

// We'll need to include the core OpenGL header specific to macOS
// after making the context current.
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

// Include GLM
// GLM is a header-only library, meaning you just need the header files.
// If you downloaded GLM, make sure your compiler can find its headers.
// A common setup is to put the 'glm' folder in your project directory or a 'libs' folder
// and tell the compiler about it with an -I flag in the Makefile (like we did for GLFW).
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Vertex Shader source code
// Using standard C++ string concatenation instead of raw string literals
const char* vertexShaderSource =
    "#version 330 core\n" // Use OpenGL version 3.3 with core profile
    "\n"
    "layout (location = 0) in vec3 aPos;\n" // Input vertex position from attribute 0
    "layout (location = 1) in vec3 aColor;\n" // Input vertex color from attribute 1
    "\n"
    "out vec3 vColor;\n" // Output color to the fragment shader
    "\n"
    "uniform mat4 model;\n" // Transformation matrix for object's position/rotation/scale
    "uniform mat4 view;\n" // Transformation matrix for camera position/orientation
    "uniform mat4 projection;\n" // Transformation matrix for perspective/orthographic view
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n" // Calculate final vertex position
    "    vColor = aColor;\n" // Pass color through
    "}\0"; // Null terminator for the string


// Fragment Shader source code
// Using standard C++ string concatenation
const char* fragmentShaderSource =
    "#version 330 core\n" // Use OpenGL version 3.3 with core profile
    "\n"
    "in vec3 vColor;\n" // Input color from the vertex shader (interpolated)
    "\n"
    "out vec4 FragColor;\n" // Output final color for the pixel
    "\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(vColor, 1.0);\n" // Set the pixel color
    "}\0"; // Null terminator for the string


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

// Helper function to link shaders into a program
unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    unsigned int program = glCreateProgram(); // Create a shader program object (ID)
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexSource); // Compile vertex shader
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource); // Compile fragment shader

    if (vs == 0 || fs == 0) {
        // If compilation failed for either, clean up and return 0
        glDeleteProgram(program);
        return 0;
    }

    glAttachShader(program, vs); // Attach vertex shader to the program
    glAttachShader(program, fs); // Attach fragment shader to the program
    glLinkProgram(program); // Link the program

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(program); // Delete the program if linking failed
        glDeleteShader(vs); // Delete shaders as they are linked into the program now
        glDeleteShader(fs);
        return 0;
    }

    // Delete the shaders as they're now linked into our program and we don't need them anymore
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program; // Return the program ID
}


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


    // --- Create a Windowed Mode Window and its OpenGL Context ---
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Cube", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // Clean up GLFW before exiting
        return -1;
    }

    // --- Make the Window's Context the Current One ---
    glfwMakeContextCurrent(window);

    // *** IMPORTANT for macOS ***
    // We need to include the OpenGL header *after* the context is current.
    // This allows the OpenGL functions to be loaded correctly.
    // Note: The warning about gl.h and gl3.h is common on macOS because
    // glfw3.h might pull in gl.h and we explicitly include gl3.h.
    // For modern OpenGL (3.3+ core profile), gl3.h is preferred.
    // This warning is usually harmless in this setup but good to be aware of.
#ifdef __APPLE__
    // #include <OpenGL/gl3.h> // Already included at the top now for clarity on placement
#endif

    // Enable depth testing - essential for 3D
    // Without this, closer objects won't correctly obscure farther objects.
    glEnable(GL_DEPTH_TEST);


    // --- Prepare Cube Data ---
    // Define the vertices of the cube.
    // Each vertex has a position (x, y, z) and a color (r, g, b).
    // We define 8 unique vertices for the corners of the cube.
    // The colors are just for visualization.
    float vertices[] = {
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

    // Define the indices for the cube.
    // This tells OpenGL which vertices from the 'vertices' array make up each triangle.
    // A cube has 6 faces, each face is made of 2 triangles.
    // Total triangles = 12. Total indices = 12 * 3 = 36.
    unsigned int indices[] = {
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


    // --- Set up OpenGL Objects (VAO, VBO, EBO) ---
    // These objects store and manage your vertex data on the GPU.
    unsigned int VAO, VBO, EBO; // IDs for Vertex Array Object, Vertex Buffer Object, Element Buffer Object

    glGenVertexArrays(1, &VAO); // Generate 1 VAO
    glGenBuffers(1, &VBO);      // Generate 1 VBO
    glGenBuffers(1, &EBO);      // Generate 1 EBO

    // 1. Bind the VAO first. Any subsequent vertex attribute calls will be stored in this VAO.
    glBindVertexArray(VAO);

    // 2. Bind the VBO and upload the vertex data to the GPU memory.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // STATIC_DRAW means data won't change often

    // 3. Bind the EBO and upload the index data to the GPU memory.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // STATIC_DRAW means data won't change often


    // 4. Configure Vertex Attributes
    // Tell OpenGL how to interpret the data in the VBO for each attribute in the vertex shader.
    // aPos (layout = 0):
    // index: 0 (matches 'layout (location = 0)' in shader)
    // size: 3 (vec3 has 3 components: x, y, z)
    // type: GL_FLOAT
    // normalized: GL_FALSE (don't normalize the data)
    // stride: 6 * sizeof(float) (the distance between the start of one vertex and the start of the next vertex in the array. Each vertex is 3 pos + 3 color = 6 floats)
    // pointer: (void*)0 (offset from the beginning of the vertex data. Positions start at the very beginning.)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Enable the vertex attribute at location 0

    // aColor (layout = 1):
    // index: 1 (matches 'layout (location = 1)' in shader)
    // size: 3 (vec3 has 3 components: r, g, b)
    // type: GL_FLOAT
    // normalized: GL_FALSE
    // stride: 6 * sizeof(float) (same stride as positions)
    // pointer: (void*)(3 * sizeof(float)) (offset to the color data within each vertex. Color starts after 3 floats of position data.)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // Enable the vertex attribute at location 1

    // 5. Unbind the VAO and VBO (and EBO by extension, since it's associated with the VAO)
    // Good practice to unbind after setup. Unbinding the VAO ensures you don't accidentally configure other buffers with this VAO active.
    // Note: The EBO *is* stored in the VAO state, so unbinding the VAO effectively unbinds the EBO from the VAO.
    // You should unbind the EBO *after* unbinding the VAO if you were planning to use EBOs for other objects later without a VAO switch.
    // In this case, unbinding the VAO is sufficient for isolating this cube's setup.
    glBindVertexArray(0); // Unbind VAO
    // glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO (optional after VAO unbind, but safe)
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO (optional after VAO unbind, but safe)


    // --- Compile and Link Shaders ---
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) {
        // Clean up buffers/arrays if shader program failed
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Get the uniform locations (IDs) for the transformation matrices in the shader
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");


    // --- Main Rendering Loop ---
    while (!glfwWindowShouldClose(window)) {
        // --- Input Handling ---
        glfwPollEvents();

        // --- Rendering Commands Go Here ---
        // Clear the color and depth buffers in each frame
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set the clear color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear both color and depth

        // Use the shader program
        glUseProgram(shaderProgram);

        // --- Create and set Transformations (Model, View, Projection) ---

        // Model Matrix: Transforms the cube from its local space (where its vertices are defined around the origin)
        // into the world space. We'll rotate it over time.
        glm::mat4 model = glm::mat4(1.0f); // Start with an identity matrix
        // Rotate the model around the X and Y axes over time for animation
        float time = glfwGetTime(); // Get current time
        model = glm::rotate(model, time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f)); // Rotate 50 degrees per second around (0.5, 1.0, 0.0) axis

        // View Matrix: Transforms vertices from world space into view space (relative to the camera).
        // It's like moving the world in front of a static camera at the origin.
        // We'll place the camera back a bit and look towards the origin.
        glm::mat4 view = glm::mat4(1.0f); // Start with an identity matrix
        // glm::lookAt(cameraPosition, cameraTarget, upVector)
        view = glm::lookAt(glm::vec3(2.0f, 2.0f, 3.0f), // Camera is at (2, 2, 3)
                           glm::vec3(0.0f, 0.0f, 0.0f), // Looking towards the origin (0, 0, 0)
                           glm::vec3(0.0f, 1.0f, 0.0f)); // Up direction is positive Y

        // Projection Matrix: Transforms vertices from view space into clip space.
        // This defines the viewing frustum (the visible 3D area).
        // We'll use a perspective projection, like a real camera.
        glm::mat4 projection = glm::mat4(1.0f); // Start with an identity matrix
        // glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane)
        projection = glm::perspective(glm::radians(45.0f), // 45 degree field of view
                                      800.0f / 600.0f,   // Aspect ratio (window width / window height)
                                      0.1f,              // Near clipping plane (objects closer than this are cut off)
                                      100.0f);           // Far clipping plane (objects farther than this are cut off)


        // Pass the transformation matrices to the shader program uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); // Set the 'model' uniform
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));  // Set the 'view' uniform
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)); // Set the 'projection' uniform


        // --- Draw the Cube ---
        // Bind the VAO that contains the cube's data and attribute configurations
        glBindVertexArray(VAO);
        // Draw the elements (triangles) using the indices in the bound EBO
        glDrawElements(GL_TRIANGLES, // What to draw (triangles)
                       36,           // How many indices to draw (36 for a cube)
                       GL_UNSIGNED_INT, // Type of indices (unsigned int)
                       0);           // Offset in the EBO (start from the beginning)

        // Unbind the VAO (optional, but good practice)
        glBindVertexArray(0);


        // --- Swap Buffers ---
        glfwSwapBuffers(window);
    }

    // --- Clean Up ---
    // Delete the OpenGL objects (VAO, VBO, EBO, Shader Program)
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // Destroy the window and its context
    glfwDestroyWindow(window);
    // Terminate GLFW
    glfwTerminate();

    return 0;
}
