#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <thread>
#include <algorithm>
#include <time.h>

#include "main.hpp"
#include "Console.hpp"
#include "Camera.h"
#include "Input.h"
#include "Texture.h"
#include "World/Chunk.h"
#include "Shader.h"

SDL_GLContext sdlGLContext = nullptr;
SDL_GameController* controller = NULL;

bool quit = false;

Camera camera;
float sensitivity = 0.5f;

static void GLClearAllErrors() {
	while (glGetError() != GL_NO_ERROR) {

	}
}

static bool GLCheckErrorStatus(const char* function, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "OpenGL Error code " << error << "\nLine " << line << "\nFunction: " << function << std::endl;
		return true;
	}
	return false;
}
#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x, __LINE__);

bool wireframe = false;
static void ToggleWireframe() {
	wireframe = !wireframe;
	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

float speed = 1.0f;
static void HandleInput() {
	SetupInput();
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			quit = true;
		}

		else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
			if (!usingController) {
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
		}
		else if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}

		if (e.type == SDL_KEYDOWN && !usingController) {
			SDL_Keycode key = e.key.keysym.sym;
			if (key == SDLK_v) {
				ToggleWireframe();
			}
		}
		if (e.type == SDL_CONTROLLERBUTTONDOWN && usingController) {
			switch (e.cbutton.button)
			{
			case SDL_CONTROLLER_BUTTON_Y:
				ToggleWireframe();
				break;
			case SDL_CONTROLLER_BUTTON_B:
				quit = true;
				break;
			default:
				break;
			}
		}
		InputEvent(e);
	}
	glm::vec3 move = glm::vec3(0.0f);
	glm::vec2 look = glm::vec2(0.0f);;
	if (usingController) {
		move.x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) / -327670.0f;
		move.z = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) / -327670.0f;
		move.y += SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 327670.0f;
		move.y -= SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 327670.0f;

		look.x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) / 16383.5f;
		look.y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) / 16383.5f;

		if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP)) {
			speed += 0.25f;
		}
		if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
			speed -= 0.25;
		}

		if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSTICK)) {
			move *= 3.0f;
		}
	}
	else {
		const Uint8* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_W]) {
			move.z += 0.05f;
		}
		if (state[SDL_SCANCODE_S]) {
			move.z -= 0.05f;
		}
		if (state[SDL_SCANCODE_A]) {
			move.x += 0.05f;
		}
		if (state[SDL_SCANCODE_D]) {
			move.x -= 0.05f;
		}
		if (state[SDL_SCANCODE_SPACE]) {
			move.y -= 0.05f;
		}
		if (state[SDL_SCANCODE_LSHIFT]) {
			move.y += 0.05f;
		}
		if (state[SDL_SCANCODE_LCTRL]) {
			move *= 3.0f;
		}
		speed += Input::getScroll() / 5.0f;
		look = Input::getMouseMoved();
	}
	if (speed < 0.1f) {
		speed = 0.1f;
	}
	if (speed > 7.0f) {
		speed = 7.0f;
	}
	move *= speed;
	camera.move(move);
	camera.addRotation(glm::vec3(look.y, look.x, 0.0f) * sensitivity);
}

GLint chunkMatrix = 0;
GLint viewMatrix = 0;
GLint projectionMatrix = 0;
static GLuint CreateShaderProgram(const std::string& vs, const std::string& fs) {
	GLuint programObject = glCreateProgram();

	Shader vertex = Shader(GL_VERTEX_SHADER);
	vertex.LoadShaderFile(vs);
	vertex.CompileShader();
	Shader fragment = Shader(GL_FRAGMENT_SHADER);
	fragment.LoadShaderFile(fs);
	fragment.CompileShader();


	vertex.AttachProgram(programObject);
	fragment.AttachProgram(programObject);
	glLinkProgram(programObject);

	glValidateProgram(programObject);

	chunkMatrix = glGetUniformLocation(programObject, "chunkMatrix");
	viewMatrix = glGetUniformLocation(programObject, "viewMatrix");
	projectionMatrix = glGetUniformLocation(programObject, "projectionMatrix");

	return programObject;
}

static void DrawChunk(Chunk* chunk) {
	glm::ivec3 blockPos = chunk->getBlockPos();
	glm::mat4 chunkMat = glm::translate(glm::mat4(1.0f), glm::vec3(blockPos.x, 0.0f, blockPos.z));
	glUniformMatrix4fv(chunkMatrix, 1, GL_FALSE, &chunkMat[0][0]);

	glBindVertexArray(chunk->getMesh()->normalVAO);
	glDrawElements(GL_TRIANGLES, chunk->getMesh()->normalIndicesSize, GL_UNSIGNED_INT, 0);

	glDepthMask(GL_FALSE);
	glBindVertexArray(chunk->getMesh()->transVAO);
	glDrawElements(GL_TRIANGLES, chunk->getMesh()->transparentIndicesSize, GL_UNSIGNED_INT, 0);
	glDepthMask(GL_TRUE);
}

Chunk* chunk;
Chunk* chunk2;
Chunk* chunk3;
Chunk* chunk4;
static void Draw() {
	glUseProgram(shaderProgram);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.4f, 0.6f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawChunk(chunk);
	DrawChunk(chunk2);
	DrawChunk(chunk3);
	DrawChunk(chunk4);
	glBindVertexArray(0);

	glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);

	glm::mat4 project = glm::perspective(glm::radians(60.0f), (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);
	glUniformMatrix4fv(projectionMatrix, 1, GL_FALSE, &project[0][0]);
	glBindVertexArray(0);

}

static void CreateBuffers() {
	long long beofre = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	ChunkMesh* mesh = chunk->getMesh();
	ChunkMesh* mesh2 = chunk2->getMesh();
	ChunkMesh* mesh3 = chunk3->getMesh();
	ChunkMesh* mesh4 = chunk4->getMesh();
	long long after = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	std::cout << (after - beofre) / 1000000.0f << "ms" << std::endl;

	chunk->genBuffers();
	chunk2->genBuffers();
	chunk3->genBuffers();
	chunk4->genBuffers();
}

static void SetupController();

static void SelectController(std::vector<int> valid) {
	std::cout << "Select controller: ";
	int selected;
	std::cin >> selected;
	if (selected > valid.size() - 1 || selected < 0) {
		std::cout << "Not a valid selection." << std::endl;
		SelectController(valid);
		return;
	}
	if (selected == 0) {
		usingController = false;
		return;
	}
	controller = SDL_GameControllerOpen(valid[selected]);
	if (!controller) {
		std::cout << "Couldnt connect to controller: " << SDL_GetError() << std::endl;
		SetupController();
	}
}

static void SetupController() {
	int count = SDL_NumJoysticks();
	std::vector<int> valid;
	valid.push_back(0);
	for (int i = 0; i < count; i++) {
		if (SDL_IsGameController(i)) {
			valid.push_back(i);
		}
	}
	std::cout << "0 - Keyboard and Mouse" << std::endl;
	for (int i = 1; i < valid.size(); i++) {
		std::cout << i << " - " << SDL_GameControllerNameForIndex(valid[i]) << std::endl;
	}
	SelectController(valid);
}

int fps = 0;
int main(int argc, char* argv[]) {
	OpenConsole();
	bool fullscreen = false;
	for (int i = 1; i < argc; ++i) {
		if (std::strcmp(argv[i], "--fullscreen") == 0) {
			fullscreen = true;
		}
	}

	//init
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
		std::cout << "SDL2 could not initialize video subsystem" << std::endl;
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SetupController();

	if (fullscreen) {
		std::cout << "Launching in fullscreen mode" << std::endl;
	}
	else {
		std::cout << "Launching in windowed mode" << std::endl;
	}

	if (fullscreen) {
		SDL_DisplayMode displayMode;
		SDL_GetDesktopDisplayMode(0, &displayMode);
		windowWidth = displayMode.w;
		windowHeight = displayMode.h;
	}

	sdlWindow = SDL_CreateWindow("OpenGL Window FPS: 0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
	if (sdlWindow == nullptr) {
		std::cout << "SDL_Window was not able to be created" << std::endl;
		exit(2);
	}
	if (fullscreen) {
		SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN);
	}

	sdlGLContext = SDL_GL_CreateContext(sdlWindow);
	if (sdlGLContext == nullptr) {
		std::cout << "OpenGL Context could not be created" << std::endl;
		exit(3);
	}

	//glad
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		std::cout << "glad was not initialized" << std::endl;
		exit(4);
	}

	SDL_GL_SetSwapInterval(1);

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::ofstream image("height.txt");

	std::string size = std::to_string(Chunk::chunkSize * 2);;
	char newLine[1] = { '\n' };
	char space[1] = { ' ' };
	image.write(size.data(), size.size());
	image.write(space, 1);
	image.write(size.data(), size.size());
	image.write(newLine, sizeof(newLine));

	chunk = new Chunk(0, 0);
	chunk2 = new Chunk(1, 0);
	chunk3 = new Chunk(0, 1);
	chunk4 = new Chunk(1, 1);
	chunk->GenerateChunk(image);
	chunk2->GenerateChunk(image);
	chunk3->GenerateChunk(image);
	chunk4->GenerateChunk(image);
	//vertex specification
	CreateBuffers();
	image.close();

	//create graphics pipeline
	shaderProgram = CreateShaderProgram("vertex.glsl", "fragment.glsl");
	Texture texture = Texture();
	texture.loadTexture("atlas.png");
	glUniform1i(glGetUniformLocation(shaderProgram, "texture"), texture.getTexture());

	if (!usingController) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	long long lastTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	long long frameCounter = 0;
	const long nanosecond = 1000000000L;
	int frames = 0;
	std::string windowTitle = "OpenGL Window FPS: ";
	//loop
	while (!quit) {
		long long startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		long long passedTime = startTime - lastTime;
		lastTime = startTime;

		frameCounter += passedTime;

		if (frameCounter >= nanosecond) {
			fps = frames;
			SDL_SetWindowTitle(sdlWindow, (windowTitle + std::to_string(frames)).data());
			frames = 0;
			frameCounter = 0;
		}
		HandleInput();
		Draw();
		SDL_GL_SwapWindow(sdlWindow);
		frames++;
	}
	//cleanup
	SDL_SetRelativeMouseMode(SDL_FALSE);
	delete chunk;
	delete chunk2;
	delete chunk3;
	delete chunk4;

	CloseConsole();
	SDL_DestroyWindow(sdlWindow);
	SDL_Quit();
	return 0;
}