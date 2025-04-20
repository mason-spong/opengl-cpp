#include "Mesh.h"

Mesh::Mesh(const float *vertices, size_t vertexSize, const unsigned int *indices, size_t indexSize, size_t vertexStride, const std::vector<std::pair<unsigned int, size_t>> &attributeLayout)
{
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
    for (const auto &attr : attributeLayout)
    {
        unsigned int location = attr.first;
        size_t offset = attr.second;
        // Assuming size=3, type=GL_FLOAT, normalized=GL_FALSE for this cube example
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, vertexStride, (void *)offset);
        glEnableVertexAttribArray(location);
    }

    glBindVertexArray(0); // Unbind VAO
    // Note: VBO and EBO are unbound when VAO is unbound
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::bind() const
{
    glBindVertexArray(VAO);
}

void Mesh::unbind() const
{
    glBindVertexArray(0);
}