#version 330 core

layout (location = 0) in vec3 InPosition;
layout (location = 1) in vec3 InColor;

out vec3 Color;

void main()
{
	Color = 1.0 - InColor;
	gl_Position	= vec4(InPosition, 1.0);
}
