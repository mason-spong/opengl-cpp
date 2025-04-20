#ifndef WORLD_H
#define WORLD_H

#include <vector> // For storing block positions

// Include GLM
#include <glm/glm.hpp>

// Simple representation of the game world holding block positions
class World
{
public:
    std::vector<glm::vec3> blockPositions;

    void addBlock(const glm::vec3 &pos);

    const std::vector<glm::vec3> &getBlocksToRender() const;
};

#endif