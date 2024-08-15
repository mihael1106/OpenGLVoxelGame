#ifndef BLOCK_H
#define BLOCK_H
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <glad/glad.h>

enum BlockFace {
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

static int nextID;
class Block {
public:
	Block() {

	}
	~Block() {

	}
	bool getOpaque() const;
	bool getFriendly() const;
	bool getEmpty() const;
	glm::ivec2 getAtlasPos(BlockFace face) const;

	bool operator==(const Block& other) const {
		return other.blockID == this->blockID;
	}

	class Builder {
	public:
		Builder() {

		}
		Builder setOpaque();
		Builder setFriendly();
		Builder setAir();
		Builder setMultiSided();
		Builder setAtlasPos(glm::ivec2 atlasPos);
		Block build();
	private:
		bool isOpaque = false;
		bool friendly = false;
		bool isEmpty = false;
		bool multiSided = false;
		glm::ivec2 atlasPos;
	};
private:
	Block(bool isOpaque, bool friendly, bool isEmpty, bool multiSided, glm::ivec2 atlasPos)
		: isOpaque(isOpaque), friendly(friendly), isEmpty(isEmpty), multiSided(multiSided), atlasPos(atlasPos)
	{
		blockID = nextID;
		nextID++;
	}
	bool isOpaque = false;
	bool friendly = false;
	bool isEmpty = true;
	bool multiSided = false;
	glm::ivec2 atlasPos;
	int blockID;
};



class BlockTypes {
public:
	static Block air;
	static Block grass;
	static Block dirt;
	static Block stone;
	static Block water;
	static Block sand;
};
#endif