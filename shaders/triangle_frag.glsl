#version 330 core

uniform sampler2D TextureSampler;
uniform sampler2D CloudsTexture;

uniform float Time;

uniform vec2 CloudsRotationSpeed = vec2(0.001, 0.0);

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

	//Vector V
	vec3 ViewDirection = vec3(0.0f, 0.0f, -1.0f);
	vec3 V = -ViewDirection;

	//Vector R(Reflection)
	vec3 R = reflect(-L, N);

	// Specular Term (R . V) ^ alpha
	float Alpha = 50.0f;
	float Specular = pow(max(dot(R, V), 0.0), Alpha);
	Specular = max(Specular, 0.0);


	//float ColorIntensity = 1.0f;
	vec3 SurfaceColor = texture(TextureSampler, UV).rgb;
	vec3 CloudColor = texture(CloudsTexture, UV + Time * CloudsRotationSpeed).rgb;
	vec3 FinalColor = (SurfaceColor + CloudColor)* LightIntensity * Lambertian + Specular;

	OutColor = vec4(FinalColor, 1.0);
}