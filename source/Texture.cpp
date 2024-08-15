#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <stb_image.h>
#include <iostream>

GLuint Texture::getTexture() const
{
	return texture;
}

glm::ivec2 Texture::getDimensions() const
{
	return dimensions;
}

void Texture::loadTexture(std::string path)
{
	stbi_set_flip_vertically_on_load(true);
	int channels;
	unsigned char* image = stbi_load(path.c_str(), &dimensions.x, &dimensions.y, &channels, 4);
	if (!image) {
		std::cout << "Failed to load image: \n" << stbi_failure_reason();
		exit(6);
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
}
