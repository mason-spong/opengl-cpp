#ifndef MESH_H
#define MESH_H

#include <cstddef>
#include <vector>
#include <utility>

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

#endif