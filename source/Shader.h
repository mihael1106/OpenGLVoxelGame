#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <glad/glad.h>

class Shader
{
public:
	Shader(GLuint type) {
		this->type = type;
	}
	void LoadShaderFile(const std::string& filename);
	void CompileShader();
	void CompileShader(const std::string& source);
	void AttachProgram(GLuint& program);
	GLuint GetShaderObject();
private:
	GLuint type;
	GLuint shaderObject = 0;
	std::string source = "";
};
#endif
