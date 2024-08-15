#include "Block.h"
#include <iostream>

Block BlockTypes::air = Block::Builder().setAir().build();
Block BlockTypes::grass = Block::Builder().setOpaque().setAtlasPos(glm::ivec2(0, 2)).setMultiSided().build();
Block BlockTypes::dirt = Block::Builder().setOpaque().setAtlasPos(glm::ivec2(0, 1)).build();
Block BlockTypes::stone = Block::Builder().setOpaque().setAtlasPos(glm::ivec2(1, 2)).build();
Block BlockTypes::water = Block::Builder().setFriendly().setAtlasPos(glm::ivec2(0, 0)).build();
Block BlockTypes::sand = Block::Builder().setOpaque().setAtlasPos(glm::ivec2(1, 1)).build();

bool Block::getOpaque() const
{
	return isOpaque;
}

bool Block::getFriendly() const
{
	return friendly;
}

bool Block::getEmpty() const
{
	return isEmpty;
}

glm::ivec2 Block::getAtlasPos(BlockFace face) const
{
	if (!multiSided) {
		return atlasPos;
	}
	glm::ivec2 faceAtlasPos = atlasPos;
	switch (face)
	{
	case FRONT:
		break;
	case BACK:
		break;
	case LEFT:
		break;
	case RIGHT:
		break;
	case TOP:
		faceAtlasPos.y += 1;
		break;
	case BOTTOM:
		faceAtlasPos.y -= 1;
		break;
	default:
		break;
	}
	return faceAtlasPos;
}

Block::Builder Block::Builder::setOpaque()
{
	isOpaque = true;
	return *this;
}

Block::Builder Block::Builder::setFriendly()
{
	friendly = true;
	return *this;
}

Block::Builder Block::Builder::setAir()
{
	isEmpty = true;
	return *this;
}

Block::Builder Block::Builder::setMultiSided()
{
	multiSided = true;
	return *this;
}

Block::Builder Block::Builder::setAtlasPos(glm::ivec2 atlasPos)
{
	this->atlasPos = atlasPos;
	return *this;
}

Block Block::Builder::build()
{
	return Block(isOpaque, friendly, isEmpty, multiSided, atlasPos);
}
