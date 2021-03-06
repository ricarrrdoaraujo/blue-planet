#include <iostream>
#include <cassert>
#include <array>
#include <fstream>
#include <vector>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int Width = 800;
int Height = 600;

std::string ReadFile(const char* FilePath)
{
	std::string FileContents;
	if (std::ifstream FileStream{ FilePath, std::ios::in })
	{
		//Read inside FileContent the content on file pointed by FilePath
		FileContents.assign(std::istreambuf_iterator<char>(FileStream), std::istreambuf_iterator<char>());
	}

	return FileContents;
};

void CheckShader(GLuint ShaderId)
{
	 // ShaderId must by a compiled shader identifier 
	GLint Result = GL_TRUE;
	glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &Result);

	if (Result == GL_FALSE)
	{
		//Error on compiling shader

		//Get log size (in bytes)
		GLint InfoLogLength = 0;
		glGetShaderiv(ShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0)
		{
			std::string ShaderInfoLog(InfoLogLength, '\0');
			glGetShaderInfoLog(ShaderId, InfoLogLength, nullptr, &ShaderInfoLog[0]);

			std::cout << "Error on shader" << std::endl;
			std::cout << ShaderInfoLog << std::endl;

			assert(false);
		}
	}
}

GLuint LoadShaders(const char* VertexShaderFile, const char* FragmentShaderFile)
{
	std::string VertexShaderSource = ReadFile(VertexShaderFile);
	std::string FragmentShaderSource = ReadFile(FragmentShaderFile);

	assert(!VertexShaderSource.empty());
	assert(!FragmentShaderSource.empty());

	//Create identifiers of Vertex and Fragment shaders
	GLuint VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	std::cout << "Compiling " << VertexShaderFile << std::endl;
	const char* VertexShaderSourcePtr = VertexShaderSource.c_str();
	glShaderSource(VertexShaderId, 1, &VertexShaderSourcePtr, nullptr);
	glCompileShader(VertexShaderId);
	//Verify if compilation was successfull
	CheckShader(VertexShaderId);

	std::cout << "Compiling " << FragmentShaderFile << std::endl;
	const char* FragmentShaderSourcePtr = FragmentShaderSource.c_str();
	glShaderSource(FragmentShaderId, 1, &FragmentShaderSourcePtr, nullptr);
	glCompileShader(FragmentShaderId);
	//Verify if compilation was successfull
	CheckShader(FragmentShaderId);

	std::cout << "Linking program" << std::endl;
	GLuint ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);
	glLinkProgram(ProgramId);

	//Verify if program was linked
	GLint Result = GL_TRUE;
	glGetProgramiv(ProgramId, GL_LINK_STATUS, &Result);

	if (Result == GL_FALSE)
	{
		

		GLint InfoLogLength = 0;
		glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0)
		{
			std::string ProgramInfoLog(InfoLogLength, '\0');
			glGetProgramInfoLog(ProgramId, InfoLogLength, nullptr, &ProgramInfoLog[0]);

			//Get log to verify issue
			std::cout << "Error on linking" << std::endl;
			std::cout << ProgramInfoLog << std::endl;

			assert(false);
		}
	}

	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(VertexShaderId);
	glDeleteShader(FragmentShaderId);

	return ProgramId;
}

GLuint LoadTexture(const char* TextureFile)
{
	std::cout << "Loading texture " << TextureFile << std::endl;

	stbi_set_flip_vertically_on_load(true);

	int TextureWidth = 0;
	int TextureHeight = 0;
	int NumberOfComponents = 0;
	//load texture on RAM memory
	unsigned char* TextureData = stbi_load(
		TextureFile,
		&TextureWidth,
		&TextureHeight,
		&NumberOfComponents,
		3);
	assert(TextureData);

	//Generate texture identifier
	GLuint TextureId;
	glGenTextures(1, &TextureId);

	// enable texture to be modified
	glBindTexture(GL_TEXTURE_2D, TextureId);

	//copy texture to GPU
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		TextureWidth,
		TextureHeight,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE, //because TextureData is unsigned char, char is one byte size
		TextureData);

	// Magnification a minification filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //solve aliasing issue

	// Texture wrapping | GL_TEXTURE_WRAP_S - axis U | GL_TEXTURE_WRAP_T - axis V
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Generate texture mipmap from texture
	glGenerateMipmap(GL_TEXTURE_2D);

	//unbind texture because is already has been copied to GPU
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(TextureData);

	return TextureId;
}

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
	glm::vec2 UV;
};

struct DirectionalLight
{
	glm::vec3 Direction;
	GLfloat Intensity;
};

GLuint LoadGeometry()
{
	//Define a triangle in normalized coordinates
	//{Position, Color, UV}
	std::array<Vertex, 6> Quad = {
		Vertex{ glm::vec3{ -1.0f, -1.0f, 0.0f },
				glm::vec3{0.0f, 0.0f, 1.0f },
				glm::vec3{1.0f, 0.0f, 0.0f},
				glm::vec2{0.0f, 0.0f}},

		Vertex{ glm::vec3{ 1.0f, -1.0f, 0.0f },
				glm::vec3{0.0f, 0.0f, 1.0f },
				glm::vec3{0.0f, 1.0f, 0.0f},
				glm::vec2{1.0f, 0.0f}},

		Vertex{ glm::vec3{ 1.0f, 1.0f, 0.0f },
				glm::vec3{0.0f, 0.0f, 1.0f },
				glm::vec3{1.0f, 0.0f, 0.0f},
				glm::vec2{1.0f, 1.0f}},

		Vertex{ glm::vec3{ -1.0f, 1.0f, 0.0f },
				glm::vec3{0.0f, 0.0f, 1.0f },
				glm::vec3{0.0f, 0.0f, 1.0f},
				glm::vec2{0.0f, 1.0f}},
	};

	//Define list of elements that compose the triangles
	std::array<glm::ivec3, 2> Indexes = {
		glm::ivec3{ 0, 1, 3 },
		glm::ivec3{ 3, 1, 2 }
	};

	//Copy triangle vertices to GPU memory
	GLuint VertexBuffer;

	// Ask OpenGL generate an identifier of VBO
	glGenBuffers(1, &VertexBuffer);

	//Ask OpenGL to generate EBO identifier
	GLuint ElementBuffer = 0;
	glGenBuffers(1, &ElementBuffer);

	// Activate VertexBuffer as the buffer where we copy the triangle data to
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	//Copy data into video memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad), Quad.data(), GL_STATIC_DRAW);

	//Copy ElementBuffer data to GPU
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indexes), Indexes.data(), GL_STATIC_DRAW);

	//Generate Vertex Array Object (VAO)
	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	//Enable VAO
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	//tells opengl that VertexBuffer will be the buffer active in the moment
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	// Inform to opengl where in the VertexBuffer are the vertexes 
	// In the case of array Triangles is contiguous in memory, is just necessary to inform how much vertexes 
	// is needed to draw the triangle
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), 
		reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, UV)));

	glBindVertexArray(0);

	return VAO;
}

void GenerateSphereMesh(
	GLuint Resolution, 
	std::vector<Vertex>& Vertexes,
	std::vector<glm::ivec3>& Indexes)
{
	Vertexes.clear();
	Indexes.clear();

	constexpr float Pi = glm::pi<float>();
	constexpr float TwoPi = glm::two_pi<float>();
	float InvResolution = 1.0f / static_cast<float>(Resolution - 1);

	//for (GLuint UIndex = 0; UIndex < Resolution; ++UIndex)
	//{
	//	const float U = UIndex * InvResolution;
	//	const float Theta = glm::mix(0.0f, Pi, U);

	//	for (GLuint VIndex = 0; VIndex < Resolution; ++VIndex)
	//	{
	//		const float V = VIndex * InvResolution;
	//		const float Phi = glm::mix(0.0f, TwoPi, V);

	//		glm::vec3 VertexPosition = {
	//			glm::sin(Theta) * glm::cos(Phi),
	//			glm::sin(Theta) * glm::sin(Phi),
	//			glm::cos(Theta)
	//		};

	//		Vertex Vertex{
	//			VertexPosition,
	//			glm::normalize(VertexPosition),
	//			glm::vec3(1.0f, 1.0f, 1.0f),
	//			glm::vec2( 1.0f - U, V)
	//		};

	//		Vertexes.push_back(Vertex);
	//	}

	//}

	for (GLuint UIndex = 0; UIndex < Resolution; ++UIndex)
	{
		const float U = UIndex * InvResolution;
		const float Theta = glm::mix(0.0f, TwoPi, static_cast<float>(U));

		for (GLuint VIndex = 0; VIndex < Resolution; ++VIndex)
		{
			const float V = VIndex * InvResolution;
			const float Phi = glm::mix(0.0f, Pi, static_cast<float>(V));

			glm::vec3 VertexPosition =
			{
				glm::cos(Theta) * glm::sin(Phi),
				glm::sin(Theta) * glm::sin(Phi),
				glm::cos(Phi)
			};

			Vertexes.push_back(Vertex{
				VertexPosition,
				glm::normalize(VertexPosition),
				glm::vec3{ 1.0f, 1.0f, 1.0f },
				glm::vec2{ 1.0f - U, V }
				});
		}
	}

	for (GLuint U = 0; U < Resolution - 1; ++U)
	{
		for (GLuint V = 0; V < Resolution - 1; ++V)
		{
			GLuint P0 = U + V * Resolution;
			GLuint P1 = (U + 1) + V * Resolution;
			GLuint P2 = (U + 1) + (V + 1) * Resolution;
			GLuint P3 = U + (V + 1) * Resolution;

			Indexes.push_back(glm::ivec3{ P0, P1, P3 });
			Indexes.push_back(glm::ivec3{ P3, P1, P2 });
		}
		
	}

}

GLuint LoadSphere(GLuint& NumVertexes, GLuint& NumIndexes)
{
	std::vector<Vertex> Vertexes;
	std::vector<glm::ivec3> Triangles;
	GenerateSphereMesh(50, Vertexes, Triangles);

	NumVertexes = Vertexes.size();
	NumIndexes = Triangles.size() * 3;

	GLuint VertexBuffer;
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, Vertexes.size() * sizeof(Vertex), Vertexes.data(), GL_STATIC_DRAW);

	GLuint ElementBuffer;
	glGenBuffers(1, &ElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				NumIndexes * sizeof(GLuint), //Triangles.size() * sizeof(glm::ivec3)
				Triangles.data(),
				GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, UV)));

	glBindVertexArray(0);

	return VAO;
}

class FlyCamera
{
public:

	void MoveForward(float Amount)
	{
		Location += glm::normalize(Direction) * Amount * Speed;
	}

	void MoveRight(float Amount)
	{
		glm::vec3 Right = glm::normalize(glm::cross(Direction, Up));
		Location += Right * Amount * Speed;
	}

	void Look(float Yaw, float Pitch)
	{
		Yaw *= Sensitivity;
		Pitch *= Sensitivity;

		glm::vec3 Right = glm::normalize(glm::cross(Direction, Up));

		const glm::mat4 I = glm::identity<glm::mat4>();
		glm::mat4 YawRotation = glm::rotate(I, glm::radians(Yaw), Up);
		glm::mat4 PitchRotation = glm::rotate(I, glm::radians(Pitch), Right);

		Up = PitchRotation * glm::vec4{ Up, 0.0f };
		Direction = YawRotation * PitchRotation * glm::vec4{ Direction, 0.0f };
	}

	glm::mat4 GetView() const
	{
		return glm::lookAt(Location, Location + Direction, Up);
	}

	glm::mat4 GetViewProjection() const
	{
		glm::mat4 Projection = glm::perspective(FieldOfView, AspectRatio, Near, Far);
		return Projection * GetView();
	}

	//Iterativity parameters
	float Speed = 5.0f;
	float Sensitivity = 0.1f;

	//View Matrix Definition
	glm::vec3 Location{ 0.0f, 0.0f, 10.0f };
	glm::vec3 Direction{ 0.0f, 0.0f, -1.0f };
	glm::vec3 Up{ 0.0f, 1.0f, 0.0f };

	//Projection Matrix definition
	float FieldOfView = glm::radians(45.0f);
	float AspectRatio = Width / Height;
	float Near = 0.01f;
	float Far = 1000.0f;
};

FlyCamera Camera;
bool bEnableMouseMovement = false;
glm::vec2 PreviousCursor{ 0.0f, 0.0f };

void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Modifiers)
{
	std::cout << "Button: " << Button <<
		" Action: " << Action <<
		" Modifiers: " << Modifiers << std::endl;

	if (Button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (Action == GLFW_PRESS)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //hide cursor
			double X, Y;
			glfwGetCursorPos(Window, &X, &Y);

			PreviousCursor = glm::vec2{ X, Y };
			bEnableMouseMovement = true;
		}

		if (Action == GLFW_RELEASE)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			bEnableMouseMovement = false;
		}
	}
				
}

void MouseMotionCallback(GLFWwindow* Window, double X, double Y)
{
	if (bEnableMouseMovement)
	{
		glm::vec2 CurrentCursor{ X, Y };
		glm::vec2 DeltaCursor = CurrentCursor - PreviousCursor;

		std::cout << glm::to_string(DeltaCursor) << std::endl;

		Camera.Look(DeltaCursor.x, DeltaCursor.y);

		PreviousCursor = CurrentCursor;
	}
	
}

void Resize(GLFWwindow* Window, int NewWidth, int NewHeight)
{
	Width = NewWidth;
	Height = NewHeight;

	Camera.AspectRatio = static_cast<float>(Width) / Height;
	glViewport(0, 0, Width, Height);
}

int main()
{

	// initialize GLFW
	assert(glfwInit() == GLFW_TRUE);

	// Create window
	GLFWwindow* Window = glfwCreateWindow(Width, Height, "Blue Planet", nullptr, nullptr);
	assert(Window);

	//sign callbacks on GLFW
	glfwSetMouseButtonCallback(Window, MouseButtonCallback);
	glfwSetCursorPosCallback(Window, MouseMotionCallback);

	//Call Resize aways when the window aspect ratio change
	glfwSetFramebufferSizeCallback(Window, Resize);

	//Activate context created on window Window
	glfwMakeContextCurrent(Window);

	//Enable or disable V-Sync
	glfwSwapInterval(1);

	//Initialize glew
	assert(glewInit() == GLEW_OK);

	//Verify OpenGL version
	GLint GLMajorVersion = 0;
	GLint GLMinorVersion = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &GLMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &GLMinorVersion);
	std::cout << "OpenGL Version: " << GLMajorVersion << "." << GLMinorVersion << std::endl;

	// Use glGetString() to obtain the current drive info
	std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	Resize(Window, Width, Height);

	GLuint ProgramId = LoadShaders("shaders/triangle_vert.glsl", "shaders/triangle_frag.glsl");

	GLuint TextureId = LoadTexture("textures/earth_2k.jpg");
	GLuint CloudTextureId = LoadTexture("textures/earth_clouds_2k.jpg");

	GLuint QuadVAO = LoadGeometry();

	GLuint SphereNumVertexes = 0;
	GLuint SphereNumIndexes = 0;
	GLuint SphereVAO = LoadSphere(SphereNumVertexes, SphereNumIndexes);

	std::cout << "Number of vertexes of sphere" << SphereNumVertexes << std::endl;
	std::cout << "Number of indexes of sphere" << SphereNumIndexes << std::endl;

	//Model Matrix
	glm::mat4 I = glm::identity<glm::mat4>();
	glm::mat4 ModelMatrix = glm::rotate(I, glm::radians(90.0f), glm::vec3{ 1, 0, 0});

	//Define background color
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	// store previous frame time
	double PreviousTime = glfwGetTime();

	// Enable Backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//enable Z-Buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Create a directional light source
	DirectionalLight Light;
	Light.Direction = glm::vec3{ 0.0f, 0.0f, -1.0f };
	Light.Intensity = 1.0f;

	// Start event loop
	while (!glfwWindowShouldClose(Window))
	{
		double CurrentTime = glfwGetTime();
		double DeltaTime = CurrentTime - PreviousTime;
		if (DeltaTime > 0.0)
		{
			PreviousTime = CurrentTime;
		}

		//Clear framebuffer. GL_COLOR_BUFFER_BIT clear color buffer and fullfil with the color defined on glClearColor
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate shader program
		glUseProgram(ProgramId);

		glm::mat4 NormalMatrix = glm::inverse(glm::transpose(Camera.GetView() * ModelMatrix));
		glm::mat4 ViewProjectionMatrix = Camera.GetViewProjection();
		glm::mat4 ModelViewProjection = ViewProjectionMatrix * ModelMatrix;

		GLint TimeLoc = glGetUniformLocation(ProgramId, "Time");
		glUniform1f(TimeLoc, CurrentTime);

		GLint ModelViewProjectionLoc = glGetUniformLocation(ProgramId, "ModelViewProjection");
		glUniformMatrix4fv(ModelViewProjectionLoc, 1, GL_FALSE, glm::value_ptr(ModelViewProjection));

		GLint NormalMatrixLoc = glGetUniformLocation(ProgramId, "NormalMatrix");
		glUniformMatrix4fv(NormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureId);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, CloudTextureId);

		GLint TextureSamplerLoc = glGetUniformLocation(ProgramId, "TextureSampler");
		glUniform1i(TextureSamplerLoc, 0);

		GLint CloudTextureLoc = glGetUniformLocation(ProgramId, "CloudsTexture");
		glUniform1i(CloudTextureLoc, 1);

		GLint LightDirectionLoc = glGetUniformLocation(ProgramId, "LightDirection");
		glUniform3fv(LightDirectionLoc, 1, 
			glm::value_ptr(Camera.GetView() * glm::vec4{ Light.Direction, 0.0f }));

		GLint LightIntensityLoc = glGetUniformLocation(ProgramId, "LightIntensity");
		glUniform1f(LightIntensityLoc, Light.Intensity);

		//glBindVertexArray(QuadVAO);
		glBindVertexArray(SphereVAO);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//glDrawArrays(GL_TRIANGLES, 0, Quad.size());
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		//glDrawArrays(GL_POINTS, 0, SphereNumVertexes);
		glDepthFunc(GL_LESS);
		glDrawElements(GL_TRIANGLES, SphereNumIndexes, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);

		//Disable active program
		glUseProgram(0);

		// Process all events on GLFW event queue 
		// Can be keyboard events, mouse or gamepad events
		glfwPollEvents();

		// Send framebuffer content of window to be draw on screen
		glfwSwapBuffers(Window);

		// Process keyboard input
		if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
		{
			Camera.MoveForward(1.0f * DeltaTime);
		}

		if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
		{
			Camera.MoveForward(-1.0f * DeltaTime);
		}

		if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
		{
			Camera.MoveRight(-1.0f * DeltaTime);
		}

		if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
		{
			Camera.MoveRight(1.0f * DeltaTime);
		}
	}

	// Unalocate VertexBuffer
	glDeleteVertexArrays(1, &QuadVAO);

	// End GLFW
	glfwTerminate();

	
	return 0;
}