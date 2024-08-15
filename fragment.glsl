#version 410 core

out vec4 color;

in vec2 in_coords;
uniform sampler2D tex;

void main()
{
    color = texture(tex, in_coords);;
}