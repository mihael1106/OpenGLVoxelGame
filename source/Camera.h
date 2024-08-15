#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
class Camera
{
public:
	void addRotation(glm::vec3 rotation);
	void setRotation(glm::vec3 rotation);
	void move(glm::vec3 amount);
	void addPosition(glm::vec3 position);
	void setPosition(glm::vec3 position);
	glm::mat4 getViewMatrix();
	glm::vec3 getPosition() {
		return position;
	}
private:
	glm::vec3 position;
	glm::vec3 rotation;
};
#endif
