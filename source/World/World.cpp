#include "World.h"
#include <iostream>

Block World::getBlock(int x, int y, int z)
{
    glm::ivec2 chunkPos = glm::ivec2(x >> 4, z >> 4);
    std::cout << chunkPos.x << " " << chunkPos.y << std::endl;
    Chunk chunk = *chunks[chunkPos.x][chunkPos.y];

    if (!chunk.Generated()) {
        chunk.GenerateChunk();
    }

    return chunk.getBlock(x & 15, y, z & 15);
}

void World::setBlock(int x, int y, int z, Block block)
{
    glm::ivec2 chunkPos = glm::ivec2(x >> 4, z >> 4);
    Chunk chunk = *chunks[chunkPos.x][chunkPos.y];

    if (!chunk.Generated()) {
        chunk.GenerateChunk();
    }

    chunk.setBlock(x & 15, y, z & 15, block);
}
