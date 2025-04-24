#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <cstdint> // Required for uint8_t

// Define the dimensions of the 3D world volume
const int WORLD_WIDTH = 16;
const int WORLD_HEIGHT = 16;
const int WORLD_DEPTH = 16;
const int WORLD_VOLUME = WORLD_WIDTH * WORLD_HEIGHT * WORLD_DEPTH;

// Simple representation of the game world holding block states
class World
{
private:
    // Use a 1D array of uint8_t to store the block states (0 for empty, 1 for block)
    // uint8_t is sufficient as we only need to store 0 or 1.
    uint8_t blocks[WORLD_WIDTH * WORLD_HEIGHT * WORLD_DEPTH];

    // Helper function to calculate the 1D index from 3D coordinates
    // Returns -1 if coordinates are out of bounds, otherwise the valid index
    int getIndex(int x, int y, int z) const;

public:
    // Constructor
    World();

    // Add a block at the specified coordinates (sets the value to 1)
    // Coordinates should be within [0, WORLD_WIDTH-1], [0, WORLD_HEIGHT-1], [0, WORLD_DEPTH-1]
    void addBlock(int x, int y, int z);

    // Remove a block at the specified coordinates (sets the value to 0)
    // Coordinates should be within [0, WORLD_WIDTH-1], [0, WORLD_HEIGHT-1], [0, WORLD_DEPTH-1]
    void removeBlock(int x, int y, int z);

    // getBlocksToRender is not implemented as requested
    // const std::vector<glm::vec3> &getBlocksToRender() const;
};

#endif