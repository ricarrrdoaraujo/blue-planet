#version 330 core

layout (location = 0) in vec3 InPosition;
layout (location = 1) in vec3 InColor;

uniform mat4 ModelViewProjection;

out vec3 Color;

void main()
{
	Color = InColor;
	gl_Position	= ModelViewProjection * vec4(InPosition, 1.0);
}
