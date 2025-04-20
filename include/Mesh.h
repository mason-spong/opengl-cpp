#ifndef MESH_H
#define MESH_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Forward declarations (optional but good practice if classes referenced only by pointer/reference)
// None needed for this specific Shader class structure

// --- Helper Functions (Declared here, implemented in .cpp) ---

// Helper function to read shader source code from a file
std::string readShaderFile(const std::string &filePath);

// Helper function to compile shaders
unsigned int compileShader(unsigned int type, const char *source);

// --- Class Definition ---

// Represents geometric data (vertices, indices) and its OpenGL buffers (VAO, VBO, EBO)
class Mesh
{
public:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount; // Number of indices to draw

    // Constructor takes vertex and index data and sets up OpenGL buffers
    Mesh(const float *vertices, size_t vertexSize, const unsigned int *indices, size_t indexSize, size_t vertexStride, const std::vector<std::pair<unsigned int, size_t>> &attributeLayout);

    // Destructor to clean up OpenGL buffers
    ~Mesh();

    // Bind the mesh's VAO for drawing
    void bind() const;

    // Unbind the mesh's VAO
    void unbind() const;
};

#endif // MESH_H