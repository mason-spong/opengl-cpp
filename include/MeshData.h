#pragma once

#include <vector>
#include <cstddef>
#include <utility>
// Use GLM for vector types, common in OpenGL projects
// You might need to install/include GLM: https://glm.g-truc.net/
#include <glm/glm.hpp>

struct MeshData
{
    std::vector<glm::vec3> vertices;   // Vertex positions (x, y, z)
    std::vector<glm::vec3> normals;    // Surface normals (for lighting)
    std::vector<glm::vec2> texCoords;  // Texture coordinates (u, v)
    std::vector<unsigned int> indices; // Indices defining triangles

    std::vector<std::tuple<unsigned int, size_t, int>> attributeLayout = {
        {0, 0, 3},                 // Pos: loc 0, offset 0, size 3
        {1, 3 * sizeof(float), 3}, // Normal: loc 1, offset 3*float, size 3
        {2, 6 * sizeof(float), 2}  // TexCoord: loc 2, offset 6*float, size 2
    };
    // Clears all data vectors
    void
    clear()
    {
        vertices.clear();
        normals.clear();
        texCoords.clear();
        indices.clear();
    }

    std::vector<float> getInterleavedVertices()
    {
        std::vector<float> interleavedData;
        for (int i = 0; i < vertices.size(); ++i)
        {
            interleavedData.emplace_back(vertices[i].x);
            interleavedData.emplace_back(vertices[i].y);
            interleavedData.emplace_back(vertices[i].z);

            interleavedData.emplace_back(normals[i].x);
            interleavedData.emplace_back(normals[i].y);
            interleavedData.emplace_back(normals[i].z);

            interleavedData.emplace_back(texCoords[i].x);
            interleavedData.emplace_back(texCoords[i].y);
        }

        return interleavedData;
    }

    size_t getVertexStride()
    {
        return 8 * sizeof(float);
    }
};