#version 330 core

uniform sampler2D TextureSampler;

in vec3 Normal;
in vec3 Color;
in vec2 UV;

uniform vec3 LightDirection;
uniform float LightIntensity;

out vec4 OutColor;

void main()
{
	//Renormalize normal to avoid problem with linear interpolation
	vec3 N = normalize(Normal);

	//Invert light direction to calculate L vector
	vec3 L = -normalize(LightDirection);

	float Lambertian = max(dot(N, L), 0.0);

	//float ColorIntensity = 1.0f;
	vec3 TextureColor = texture(TextureSampler, UV).rgb;
	vec3 FinalColor = TextureColor * LightIntensity * Lambertian;

	OutColor = vec4(FinalColor, 1.0);
}