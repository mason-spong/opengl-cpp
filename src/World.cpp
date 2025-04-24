#include "World.h"
#include <iostream> // For potential error reporting (optional)

// Helper function to calculate the 1D index from 3D coordinates
int World::getIndex(int x, int y, int z) const
{
    if (x < 0 || x >= WORLD_WIDTH ||
        y < 0 || y >= WORLD_HEIGHT ||
        z < 0 || z >= WORLD_DEPTH)
    {
        // Coordinates are out of bounds
        // In a real application, you might want more robust error handling
        std::cerr << "Warning: Coordinates (" << x << ", " << y << ", " << z << ") are out of bounds." << std::endl;
        return -1; // Indicate invalid index
    }
    // Calculate the 1D index (using y-major ordering)
    // index = y * (DEPTH * WIDTH) + z * WIDTH + x
    return y * (WORLD_DEPTH * WORLD_WIDTH) + z * WORLD_WIDTH + x;
}

// Constructor
World::World() : blocks{}
{
}

// Add a block at the specified coordinates (sets the value to 1)
void World::addBlock(int x, int y, int z)
{
    int index = getIndex(x, y, z);
    if (index != -1)
    {
        // Set the value to 1 (which fits in uint8_t)
        blocks[index] = 1;
    }
}

// Remove a block at the specified coordinates (sets the value to 0)
void World::removeBlock(int x, int y, int z)
{
    int index = getIndex(x, y, z);
    if (index != -1)
    {
        // Set the value to 0 (which fits in uint8_t)
        blocks[index] = 0;
    }
}
