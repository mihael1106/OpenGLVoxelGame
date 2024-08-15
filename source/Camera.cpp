#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

void Camera::addRotation(glm::vec3 rotation)
{
	this->rotation += rotation;
	if (this->rotation.x > 89.0f)
		this->rotation.x = 89.0f;
	if (this->rotation.x < -89.0f)
		this->rotation.x = -89.0f;
}

void Camera::setRotation(glm::vec3 rotation)
{
	this->rotation = rotation;
}

void Camera::move(glm::vec3 amount)
{
	if (amount.z != 0) {
		position.x += (float)std::sin(glm::radians(rotation.y)) * -1.0f * amount.z;
		position.z += (float)std::cos(glm::radians(rotation.y)) * amount.z;
	}
	if (amount.x != 0) {
		position.x += (float)std::sin(glm::radians(rotation.y - 90.0f)) * -1.0f * amount.x;
		position.z += (float)std::cos(glm::radians(rotation.y - 90.0f)) * amount.x;
	}
	position.y += amount.y;
}

void Camera::addPosition(glm::vec3 position)
{
	this->position += position;
}

void Camera::setPosition(glm::vec3 position)
{
	this->position = position;
}

glm::mat4 Camera::getViewMatrix()
{
	glm::mat4 viewMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	viewMatrix = glm::translate(viewMatrix, position);
	return viewMatrix;
}
