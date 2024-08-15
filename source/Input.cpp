#include "Input.h"
#include "main.hpp"
#include <iostream>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

glm::vec2 offset = glm::vec2();
float scroll = 0.0f;

static void mouse_callback(SDL_Event& e) {
	offset.x = e.motion.xrel;
	offset.y = e.motion.yrel;
	if (!usingController) {
		SDL_WarpMouseInWindow(sdlWindow, windowWidth / 2, windowHeight / 2);
	}
}

glm::vec2 Input::getMouseMoved()
{
	return offset;
}

float Input::getScroll() {
	return scroll;
}

void InputEvent(SDL_Event& event) {
	if (event.type == SDL_MOUSEMOTION) {
		mouse_callback(event);
	}
	else if (event.type == SDL_MOUSEWHEEL) {
		scroll = event.wheel.y;
	}
}

void SetupInput() {
	offset.x = 0;
	offset.y = 0;
	scroll = 0.0f;
}
