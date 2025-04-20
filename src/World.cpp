#include "World.h"

void World::addBlock(const glm::vec3 &pos)
{
    blockPositions.push_back(pos);
}

const std::vector<glm::vec3> &World::getBlocksToRender() const
{
    // In a real game, this would return visible blocks (e.g., based on camera frustum, chunks)
    return blockPositions;
}