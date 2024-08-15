#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
class Texture
{
public:
	GLuint getTexture() const;
	glm::ivec2 getDimensions() const;
	void loadTexture(std::string path);
private:
	GLuint texture;
	glm::ivec2 dimensions;
};
#endif