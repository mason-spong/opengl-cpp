#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <cstdint>
#include <iostream> // Included for error reporting in getIndex

const int WORLD_WIDTH = 16;
const int WORLD_HEIGHT = 16;
const int WORLD_DEPTH = 16;
const int WORLD_VOLUME = WORLD_WIDTH * WORLD_HEIGHT * WORLD_DEPTH;

enum class BlockType : uint8_t
{
    AIR = 0,
    DIRT = 1,
    STONE = 2,
    SAND = 3,
    GRASS = 4,
    WOOD_OAK = 5,
    COBBLESTONE = 6,
    OAK_PLANK = 7,
    OAK_LEAF = 8,
};

class World
{
private:
    std::vector<BlockType> blocks;
    int getIndex(int x, int y, int z) const;

public:
    World();
    void addBlock(int x, int y, int z, BlockType BlockType);
    void removeBlock(int x, int y, int z); // set to AIR
    BlockType getBlockType(int x, int y, int z) const;

    // --- New Method Declaration ---
    // Check if a block is solid (non-zero) at the specified coordinates
    // Returns false if out of bounds.
    bool isSolid(int x, int y, int z) const;
    // ----------------------------

    // getBlocksToRender is not implemented as requested
    // const std::vector<glm::vec3> &getBlocksToRender() const;
};

#endif