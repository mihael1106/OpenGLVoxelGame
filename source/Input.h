#ifndef INPUT_H
#define INPUT_H
#include <glm/glm.hpp>
#include <SDL.h>
void SetupInput();
void InputEvent(SDL_Event& event);
class Input
{
public:
	static glm::vec2 getMouseMoved();
	static float getScroll();
};
#endif
