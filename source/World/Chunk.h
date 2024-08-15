#ifndef CHUNK_H
#define CHUNK_H
#include "Block.h"
#include "fstream"
struct ChunkMesh
{
public:
	ChunkMesh(std::vector<GLfloat> vertices, std::vector<GLuint> indices, std::vector<GLfloat> transVertices, std::vector<GLuint> transIndices)
		: normalVertices(vertices), normalIndices(indices), transparentVertices(transVertices), transparentIndices(transIndices) 
	{
		normalVerticesSize = normalVertices.size();
		normalIndicesSize = normalIndices.size();
		transparentVerticesSize = transparentVertices.size();
		transparentIndicesSize = transparentIndices.size();
	}
	std::vector<GLfloat> normalVertices;
	std::vector<GLuint> normalIndices;
	std::vector<GLfloat> transparentVertices;
	std::vector<GLuint> transparentIndices;

	GLuint normalVerticesSize;
	GLuint normalIndicesSize;
	GLuint transparentVerticesSize;
	GLuint transparentIndicesSize;

	GLuint normalVAO = 0;
	GLuint normalVBO = 0;
	GLuint normalIBO = 0;

	GLuint transVAO = 0;
	GLuint transVBO = 0;
	GLuint transIBO = 0;
};

class Chunk
{
public:
	static const int chunkSize = 16;
	Chunk(int chunkX, int chunkZ) : chunkX(chunkX), chunkZ(chunkZ) {
	}
	~Chunk() {
		delete mesh;
	}
	const Block& getBlock(int x, int y, int z) const;
	void setBlock(int x, int y, int z, Block& block);
	void GenerateChunk(std::ofstream& image);
	void genBuffers();
	glm::ivec3 getBlockPos() {
		return glm::ivec3(chunkX * chunkSize, 0, chunkZ * chunkSize);
	}
	ChunkMesh* getMesh() {
		if (!chunkGenerated) {
			//GenerateChunk();
			chunkGenerated = true;
		}
		if (!meshGenerated) {
			mesh = new ChunkMesh(generateChunkMesh(*this));
			meshGenerated = true;
		}
		return mesh;
	}
private:
	int chunkX = 0;
	int chunkZ = 0;
	Block blocks[chunkSize][chunkSize][chunkSize];
	bool meshGenerated = false;
	bool chunkGenerated = false;
	ChunkMesh* mesh = nullptr;
	ChunkMesh generateChunkMesh(Chunk& chunk);
};
#endif
