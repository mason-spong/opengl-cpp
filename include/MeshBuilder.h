#pragma once // Or remove if in .cpp

#include "MeshData.h"
#include "World.h" // The updated World class definition
#include <glm/glm.hpp>
#include <vector>

namespace MeshBuilder
{

    // Helper function: Appends the vertices and indices for a single cube
    // centered at 'centerOffset' to the provided MeshData.
    // Assumes standard cube size of 1.0f.
    void appendCube(MeshData &meshData, const glm::vec3 &centerOffset, std::map<BlockType, FaceToLayer> &layer_mapping, BlockType blockType, float size = 1.0f)
    {
        float halfSize = size / 2.0f;

        // Calculate the base index for vertices BEFORE adding this cube's vertices
        unsigned int baseVertexIndex = static_cast<unsigned int>(meshData.vertices.size());

        // Define the 8 corners RELATIVE to the centerOffset
        glm::vec3 p_rrr = centerOffset + glm::vec3(halfSize, halfSize, halfSize);    // +X, +Y, +Z
        glm::vec3 p_rrl = centerOffset + glm::vec3(halfSize, halfSize, -halfSize);   // +X, +Y, -Z
        glm::vec3 p_rlr = centerOffset + glm::vec3(halfSize, -halfSize, halfSize);   // +X, -Y, +Z
        glm::vec3 p_rll = centerOffset + glm::vec3(halfSize, -halfSize, -halfSize);  // +X, -Y, -Z
        glm::vec3 p_lrr = centerOffset + glm::vec3(-halfSize, halfSize, halfSize);   // -X, +Y, +Z
        glm::vec3 p_lrl = centerOffset + glm::vec3(-halfSize, halfSize, -halfSize);  // -X, +Y, -Z
        glm::vec3 p_llr = centerOffset + glm::vec3(-halfSize, -halfSize, halfSize);  // -X, -Y, +Z
        glm::vec3 p_lll = centerOffset + glm::vec3(-halfSize, -halfSize, -halfSize); // -X, -Y, -Z

        // Normals (remain the same regardless of offset)
        glm::vec3 n_front = {0.0f, 0.0f, 1.0f};   // +Z
        glm::vec3 n_back = {0.0f, 0.0f, -1.0f};   // -Z
        glm::vec3 n_right = {1.0f, 0.0f, 0.0f};   // +X
        glm::vec3 n_left = {-1.0f, 0.0f, 0.0f};   // -X
        glm::vec3 n_top = {0.0f, 1.0f, 0.0f};     // +Y
        glm::vec3 n_bottom = {0.0f, -1.0f, 0.0f}; // -Y

        // UV Coordinates (standard for each face)
        glm::vec2 uv_bl = {0.0f, 0.0f};
        glm::vec2 uv_br = {1.0f, 0.0f};
        glm::vec2 uv_tr = {1.0f, 1.0f};
        glm::vec2 uv_tl = {0.0f, 1.0f};

        // Texture layer
        FaceToLayer block_layer_map = layer_mapping[blockType];

        // Temporary storage for this cube's data
        std::vector<glm::vec3> cubeVertices;
        std::vector<glm::vec3> cubeNormals;
        std::vector<glm::vec2> cubeTexCoords;
        std::vector<float> cubeLayerIndices;
        std::vector<unsigned int> cubeIndices;

        // Reserve space for efficiency (24 vertices, 36 indices)
        cubeVertices.reserve(24);
        cubeNormals.reserve(24);
        cubeTexCoords.reserve(24);
        cubeLayerIndices.reserve(24);
        cubeIndices.reserve(36);

        // Add vertices, normals, UVs, texture layer for each face (CCW from outside)
        // Front (+Z)
        cubeVertices.push_back(p_llr);
        cubeVertices.push_back(p_rlr);
        cubeVertices.push_back(p_rrr);
        cubeVertices.push_back(p_lrr);
        for (int i = 0; i < 4; ++i)
            cubeNormals.push_back(n_front);
        cubeTexCoords.push_back(uv_bl);
        cubeTexCoords.push_back(uv_br);
        cubeTexCoords.push_back(uv_tr);
        cubeTexCoords.push_back(uv_tl);
        for (int i = 0; i < 4; ++i)
        { // Add index 4 times
            cubeLayerIndices.push_back(static_cast<float>(block_layer_map.front));
        }
        // Back (-Z)
        cubeVertices.push_back(p_rll);
        cubeVertices.push_back(p_lll);
        cubeVertices.push_back(p_lrl);
        cubeVertices.push_back(p_rrl);
        for (int i = 0; i < 4; ++i)
            cubeNormals.push_back(n_back);
        cubeTexCoords.push_back(uv_bl);
        cubeTexCoords.push_back(uv_br);
        cubeTexCoords.push_back(uv_tr);
        cubeTexCoords.push_back(uv_tl);
        for (int i = 0; i < 4; ++i)
        { // Add index 4 times
            cubeLayerIndices.push_back(static_cast<float>(block_layer_map.back));
        } // Right (+X)
        cubeVertices.push_back(p_rll);
        cubeVertices.push_back(p_rlr);
        cubeVertices.push_back(p_rrr);
        cubeVertices.push_back(p_rrl);
        for (int i = 0; i < 4; ++i)
            cubeNormals.push_back(n_right);
        cubeTexCoords.push_back(uv_bl);
        cubeTexCoords.push_back(uv_br);
        cubeTexCoords.push_back(uv_tr);
        cubeTexCoords.push_back(uv_tl);
        for (int i = 0; i < 4; ++i)
        { // Add index 4 times
            cubeLayerIndices.push_back(static_cast<float>(block_layer_map.right));
        } // Left (-X)
        cubeVertices.push_back(p_llr);
        cubeVertices.push_back(p_lll);
        cubeVertices.push_back(p_lrl);
        cubeVertices.push_back(p_lrr);
        for (int i = 0; i < 4; ++i)
            cubeNormals.push_back(n_left);
        cubeTexCoords.push_back(uv_bl);
        cubeTexCoords.push_back(uv_br);
        cubeTexCoords.push_back(uv_tr);
        cubeTexCoords.push_back(uv_tl);
        for (int i = 0; i < 4; ++i)
        { // Add index 4 times
            cubeLayerIndices.push_back(static_cast<float>(block_layer_map.left));
        } // Top (+Y)
        cubeVertices.push_back(p_lrl);
        cubeVertices.push_back(p_rrl);
        cubeVertices.push_back(p_rrr);
        cubeVertices.push_back(p_lrr);
        for (int i = 0; i < 4; ++i)
            cubeNormals.push_back(n_top);
        cubeTexCoords.push_back(uv_bl);
        cubeTexCoords.push_back(uv_br);
        cubeTexCoords.push_back(uv_tr);
        cubeTexCoords.push_back(uv_tl);
        for (int i = 0; i < 4; ++i)
        { // Add index 4 times
            cubeLayerIndices.push_back(static_cast<float>(block_layer_map.top));
        }
        // Bottom (-Y)
        cubeVertices.push_back(p_llr);
        cubeVertices.push_back(p_rlr);
        cubeVertices.push_back(p_rll);
        cubeVertices.push_back(p_lll);
        for (int i = 0; i < 4; ++i)
            cubeNormals.push_back(n_bottom);
        cubeTexCoords.push_back(uv_bl);
        cubeTexCoords.push_back(uv_br);
        cubeTexCoords.push_back(uv_tr);
        cubeTexCoords.push_back(uv_tl);
        for (int i = 0; i < 4; ++i)
        { // Add index 4 times
            cubeLayerIndices.push_back(static_cast<float>(block_layer_map.bottom));
        }
        // Add indices relative to the start of *this cube's* vertices (0-23)
        for (unsigned int i = 0; i < 6; ++i)
        { // For each face (which added 4 vertices)
            unsigned int faceBase = i * 4;
            cubeIndices.push_back(faceBase + 0);
            cubeIndices.push_back(faceBase + 1);
            cubeIndices.push_back(faceBase + 2);
            cubeIndices.push_back(faceBase + 0);
            cubeIndices.push_back(faceBase + 2);
            cubeIndices.push_back(faceBase + 3);
        }

        // --- Append this cube's data to the main MeshData ---

        // Append vertices, normals, texCoords, layer indices
        meshData.vertices.insert(meshData.vertices.end(), cubeVertices.begin(), cubeVertices.end());
        meshData.normals.insert(meshData.normals.end(), cubeNormals.begin(), cubeNormals.end());
        meshData.texCoords.insert(meshData.texCoords.end(), cubeTexCoords.begin(), cubeTexCoords.end());
        meshData.layerIndices.insert(meshData.layerIndices.end(), cubeLayerIndices.begin(), cubeLayerIndices.end());

        // Append indices, making sure to offset them by baseVertexIndex
        for (unsigned int index : cubeIndices)
        {
            meshData.indices.push_back(baseVertexIndex + index);
        }
    }

    // ---- The Main Function to Generate the World Mesh ----
    // Fulfills the role of the original `generateMesh(const World& world, MeshData& meshData)` signature.
    void generateWorldMesh(const World &world, MeshData &meshData, std::map<BlockType, FaceToLayer> &layer_mapping)
    {
        // Start with an empty mesh for the entire world
        meshData.clear();

        // Iterate through every voxel position in the world
        for (int y = 0; y < WORLD_HEIGHT; ++y)
        {
            for (int z = 0; z < WORLD_DEPTH; ++z)
            {
                for (int x = 0; x < WORLD_WIDTH; ++x)
                {

                    // Check if the current voxel position contains a solid block
                    if (world.isSolid(x, y, z))
                    {

                        // Calculate the world space center position of this voxel's cube
                        // (Assuming voxel coords [x,y,z] refer to the minimum corner)
                        glm::vec3 blockCenter = {
                            static_cast<float>(x) + 0.5f,
                            static_cast<float>(y) + 0.5f,
                            static_cast<float>(z) + 0.5f};

                        // Append the full geometry (all 6 faces) for a cube
                        // at this position to the main meshData object.
                        appendCube(meshData, blockCenter, layer_mapping, world.getBlockType(x, y, z), 1.0f);
                    }
                }
            }
        }
        // At this point, meshData contains the combined geometry
        // of all solid blocks, with no optimizations (hidden faces included).
    }
};