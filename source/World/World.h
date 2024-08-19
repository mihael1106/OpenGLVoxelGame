#ifndef WORLD_H
#define WORLD_H
#include "Chunk.h"

class World
{
public:
	World() {
		for (int x = 0; x < 128; x++) {
			for (int y = 0; y < 128; y++) {
				chunks[x][y] = new Chunk(x, y);
			}
		}
	}

	~World() {
		for (int x = 0; x < 128; x++) {
			for (int y = 0; y < 128; y++) {
				delete chunks[x][y];
				chunks[x][y] = nullptr;
			}
		}
	}

	Block getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block block);

	Chunk* getChunk(int chunkX, int chunkY) {
		return chunks[chunkX][chunkY];
	}

private:
	Chunk* chunks[128][128];
};

#endif
