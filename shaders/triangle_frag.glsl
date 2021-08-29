#version 330 core

in vec3 Color;
in vec2 UV;

out vec4 OutColor;

void main()
{
	//OutColor = vec4(Color, 1.0);
	OutColor = vec4(UV, 0.0, 1.0);
}