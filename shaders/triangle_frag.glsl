#version 330 core

uniform sampler2D TextureSampler;

in vec3 Color;
in vec2 UV;

out vec4 OutColor;

void main()
{
	float ColorIntensity = 1.0f;
	vec3 TextureColor = texture(TextureSampler, UV).rgb;
	vec3 FinalColor = ColorIntensity * TextureColor;

	OutColor = vec4(FinalColor, 1.0);
}