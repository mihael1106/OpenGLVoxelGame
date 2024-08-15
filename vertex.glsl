#version 410 core

layout(location=0) in vec3 pos;
layout(location=1) in vec2 coords;

uniform mat4 chunkMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 in_coords;

void main()
{
	in_coords = coords;
	gl_Position = projectionMatrix * viewMatrix * chunkMatrix * vec4(pos, 1.0f);
}