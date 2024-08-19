#include "Chunk.h"
#include <unordered_map>
#include <array>
#include <iostream>
#include <time.h>
#include <glm/gtc/noise.hpp>
#include <fstream>
#include <string> 


const Block& Chunk::getBlock(int x, int y, int z) const
{
	return blocks[x][y][z];
}

void Chunk::setBlock(int x, int y, int z, Block& block)
{
	blocks[x][y][z] = block;
}

const std::array<glm::vec3, 8> blockVertices{
	glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 0),
	glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), glm::vec3(0, 1, 1)
};

const std::array<std::array<int, 4>, 6> blockIndices{ {
	{4, 5, 6, 7}, // FRONT
	{1, 0, 3, 2}, // BACK
	{0, 4, 7, 3}, // LEFT
	{5, 1, 2, 6}, // RIGHT
	{7, 6, 2, 3}, // TOP
	{0, 1, 5, 4}  // BOTTOM
} };

static bool isFaceVisible(Chunk& chunk, int x, int y, int z, BlockFace face, const Block& block) {
	Block other = BlockTypes::air;
	switch (face) {
	case FRONT:
		if (z >= Chunk::chunkSize - 1) {
			return true;
		}
		other = chunk.getBlock(x, y, z + 1);
		if (other.getFriendly() && !other.getOpaque() && block.getFriendly() && !block.getOpaque()) {
			if (other == block) {
				return false;
			}
		}
		return !other.getOpaque();
	case BACK:
		if (z <= 0) {
			return true;
		}
		other = chunk.getBlock(x, y, z - 1);
		if (other.getFriendly() && !other.getOpaque() && block.getFriendly() && !block.getOpaque()) {
			if (other == block) {
				return false;
			}
		}
		return !other.getOpaque();
	case LEFT:
		if (x <= 0) {
			return true;
		}
		other = chunk.getBlock(x - 1, y, z);
		if (other.getFriendly() && !other.getOpaque() && block.getFriendly() && !block.getOpaque()) {
			if (other == block) {
				return false;
			}
		}
		return !other.getOpaque();
	case RIGHT:
		if (x >= Chunk::chunkSize - 1) {
			return true;
		}
		other = chunk.getBlock(x + 1, y, z);
		if (other.getFriendly() && !other.getOpaque() && block.getFriendly() && !block.getOpaque()) {
			if (other == block) {
				return false;
			}
		}
		return !other.getOpaque();
	case TOP:
		if (y >= Chunk::chunkSize - 1) {
			return true;
		}
		other = chunk.getBlock(x, y + 1, z);
		if (other.getFriendly() && !other.getOpaque() && block.getFriendly() && !block.getOpaque()) {
			if (other == block) {
				return false;
			}
		}
		return !other.getOpaque();
	case BOTTOM:
		if (y <= 0) {
			return true;
		}
		other = chunk.getBlock(x, y - 1, z);
		if (other.getFriendly() && !other.getOpaque() && block.getFriendly() && !block.getOpaque()) {
			if (other == block) {
				return false;
			}
		}
		return !other.getOpaque();
	default:
		return false;
	}
}

std::array<glm::vec2, 4> faceTexCoords = {
	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(0.0f, 1.0f)
};

ChunkMesh Chunk::generateChunkMesh(Chunk& chunk) {
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	std::vector<GLfloat> transparentVertices;
	std::vector<GLuint> transparentIndices;
	auto addFace = [&](int x, int y, int z, BlockFace face, Block block, std::vector<GLfloat>& vertexVector, std::vector<GLuint>& indexVector) {
		GLuint faceIndices[4];

		for (int i = 0; i < 4; ++i) {
			glm::vec3 position = blockVertices[blockIndices[face][i]] + glm::vec3(x, y, z);

			faceIndices[i] = vertexVector.size() / 5;

			vertexVector.push_back(position.x);
			vertexVector.push_back(position.y);
			vertexVector.push_back(position.z);

			glm::vec2 texPos = block.getAtlasPos(face);
			texPos *= 16.0f / 64.0f;

			glm::vec2 finalTexCoord = texPos + faceTexCoords[i] * 16.0f / 64.0f;

			vertexVector.push_back(finalTexCoord.x);
			vertexVector.push_back(finalTexCoord.y);
		}

		indexVector.push_back(faceIndices[0]);
		indexVector.push_back(faceIndices[1]);
		indexVector.push_back(faceIndices[2]);
		indexVector.push_back(faceIndices[2]);
		indexVector.push_back(faceIndices[3]);
		indexVector.push_back(faceIndices[0]);
		};

	for (int x = 0; x < chunkSize; ++x) {
		for (int y = 0; y < chunkSize; ++y) {
			for (int z = 0; z < chunkSize; ++z) {
				const Block block = chunk.getBlock(x, y, z);
				if (block.getEmpty()) continue;

				for (int face = 0; face < 6; ++face) {
					if (isFaceVisible(chunk, x, y, z, static_cast<BlockFace>(face), block)) {
						if (!block.getOpaque()) {
							addFace(x, y, z, static_cast<BlockFace>(face), block, transparentVertices, transparentIndices);
						}
						else {
							addFace(x, y, z, static_cast<BlockFace>(face), block, vertices, indices);
						}
					}
				}
			}
		}
	}

	meshGenerated = true;
	return ChunkMesh(vertices, indices, transparentVertices, transparentIndices);
}

void Chunk::GenerateChunk() {
	glm::ivec3 blockPos = getBlockPos();
	for (int x = 0; x < chunkSize; x++) {
		for (int z = 0; z < chunkSize; z++) {
			int worldX = blockPos.x + x;
			int worldZ = blockPos.z + z;

			float upTo = glm::perlin(glm::vec2((float)worldX / 16, (float)worldZ / 16)) * 8 + 8;

			for (int y = upTo; y >= 0; y--) {
				int difference = (int)upTo - y - 1;
				if (difference == 0) {
					setBlock(x, y, z, BlockTypes::grass);
				}
				else if (difference > 0 && difference < 4) {
					setBlock(x, y, z, BlockTypes::dirt);
				}
				else {
					setBlock(x, y, z, BlockTypes::stone);
				}
			}
			for (int y = upTo; y < chunkSize; y++) {
				setBlock(x, y, z, BlockTypes::air);
			}
		}
	}
	chunkGenerated = true;
}

void Chunk::genBuffers() {
	if (mesh == nullptr) {
		return;
	}
	glGenVertexArrays(1, &mesh->normalVAO);
	glBindVertexArray(mesh->normalVAO);

	glGenBuffers(1, &mesh->normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->normalVBO);
	glBufferData(GL_ARRAY_BUFFER, mesh->normalVerticesSize * sizeof(GL_FLOAT), mesh->normalVertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &mesh->normalIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->normalIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->normalIndicesSize * sizeof(GL_UNSIGNED_INT), mesh->normalIndices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (void*)(sizeof(GL_FLOAT) * 3));

	glBindVertexArray(0);

	glGenVertexArrays(1, &mesh->transVAO);
	glBindVertexArray(mesh->transVAO);

	glGenBuffers(1, &mesh->transVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->transVBO);
	glBufferData(GL_ARRAY_BUFFER, mesh->transparentVerticesSize * sizeof(GL_FLOAT), mesh->transparentVertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &mesh->transIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->transIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->transparentIndicesSize * sizeof(GL_UNSIGNED_INT), mesh->transparentIndices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (void*)(sizeof(GL_FLOAT) * 3));

	glBindVertexArray(0);
	buffersReady = true;
}