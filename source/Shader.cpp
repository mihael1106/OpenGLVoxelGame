#include "Shader.h"
#include <fstream>

void Shader::LoadShaderFile(const std::string& filename)
{
	std::string result = "";

	std::string line = "";
	std::ifstream shaderFile(filename.c_str());

	if (shaderFile.is_open()) {
		while (std::getline(shaderFile, line)) {
			result += line + "\n";
		}
		shaderFile.close();
	}
	source = result;
}

void Shader::CompileShader()
{
	shaderObject = glCreateShader(type);

	const char* src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	glCompileShader(shaderObject);
}

void Shader::CompileShader(const std::string& source)
{
	shaderObject = glCreateShader(type);

	const char* src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	glCompileShader(shaderObject);
}

void Shader::AttachProgram(GLuint& program)
{
	glAttachShader(program, shaderObject);
}

GLuint Shader::GetShaderObject()
{
	return shaderObject;
}
