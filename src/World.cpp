#include "World.h"
// #include <iostream> // Already included via World.h indirectly

// --- getIndex implementation (from your provided code) ---
int World::getIndex(int x, int y, int z) const
{
    if (x < 0 || x >= WORLD_WIDTH ||
        y < 0 || y >= WORLD_HEIGHT ||
        z < 0 || z >= WORLD_DEPTH)
    {
        // Coordinates are out of bounds (no error message needed here, isSolid handles it)
        return -1; // Indicate invalid index
    }
    return y * (WORLD_DEPTH * WORLD_WIDTH) + z * WORLD_WIDTH + x;
}

// --- Constructor (from your provided code) ---
World::World() : blocks(WORLD_VOLUME, BlockType::AIR) {}

// --- addBlock (from your provided code) ---
void World::addBlock(int x, int y, int z, BlockType blockType)
{
    int index = getIndex(x, y, z);
    if (index != -1)
    {
        blocks[index] = blockType;
    }
}

// --- removeBlock (from your provided code) ---
void World::removeBlock(int x, int y, int z)
{
    int index = getIndex(x, y, z);
    if (index != -1)
    {
        blocks[index] = BlockType::AIR;
    }
}

// --- New Method Definition ---
bool World::isSolid(int x, int y, int z) const
{
    int index = getIndex(x, y, z);
    // Consider out-of-bounds as not solid (air)
    if (index == -1)
    {
        return false;
    }
    // Check if the block value is not AIR
    return blocks[index] != BlockType::AIR;
}