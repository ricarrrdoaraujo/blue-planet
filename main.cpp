#include <iostream>
#include <cassert>
#include <array>
#include <fstream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

const int Width = 800;
const int Height = 600;

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

int main()
{

	// initialize GLFW
	assert(glfwInit() == GLFW_TRUE);

	// Create window
	GLFWwindow* Window = glfwCreateWindow(Width, Height, "Blue Planet", nullptr, nullptr);
	assert(Window);

	//Activate context created on window Window
	glfwMakeContextCurrent(Window);

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

	GLuint ProgramId = LoadShaders("shaders/triangle_vert.glsl", "shaders/triangle_frag.glsl");

	//Define a triangle in normalized coordinates
	std::array<glm::vec3, 3> Triangle = {
		glm::vec3{ -1.0f, -1.0f, 0.0f },
		glm::vec3{ 1.0f, -1.0f, 0.0f },
		glm::vec3{ 0.0f, 1.0f, 0.0f }
	};

	//Model Matrix
	glm::mat4 ModelMatrix = glm::identity<glm::mat4>();

	//View Matrix
	glm::vec3 Eye{ 0, 0, 5 };
	glm::vec3 Center{ 0, 0, 0 };
	glm::vec3 Up{0, 1, 0};
	glm::mat4 ViewMatrix = glm::lookAt(Eye, Center, Up);

	// Projection Matrix
	constexpr float FoV = glm::radians(45.0f);
	const float AspectRatio = Width / Height;
	const float Near = 0.001f;
	const float Far = 1000.0f;
	glm::mat4 ProjectionMatrix = glm::perspective(FoV, AspectRatio, Near, Far);

	// ModelViewProjection
	glm::mat4 ModelViewProjection = ProjectionMatrix * ViewMatrix * ModelMatrix;

	//Apply ModelViewProjection on triangle vertexes
	for (glm::vec3& Vertex : Triangle)
	{
		glm::vec4 ProjectedVertex = ModelViewProjection * glm::vec4{ Vertex, 1.0f };
		ProjectedVertex /= ProjectedVertex.w;
		Vertex = ProjectedVertex;
	}

	//Copy triangle vertices to GPU memory
	GLuint VertexBuffer;

	// Ask OpenGL generate an identifier of VertexBuffer
	glGenBuffers(1, &VertexBuffer);

	// Activate VertexBuffer as the buffer where we copy the triangle data to
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	//Copy data into video memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle.data(), GL_STATIC_DRAW);

	//Define background color
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	// Start event loop
	while (!glfwWindowShouldClose(Window))
	{
		//Clear framebuffer. GL_COLOR_BUFFER_BIT clear color buffer and fullfil with the color defined on glClearColor
		glClear(GL_COLOR_BUFFER_BIT);

		// Activate shader program
		glUseProgram(ProgramId);

		glEnableVertexAttribArray(0);
		
		//tells opengl that VertexBuffer will be the buffer active in the moment
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

		// Inform to opengl where in the VertexBuffer are the vertexes 
		// In the case of array Triangles is contiguous in memory, is just necessary to inform how much vertexes 
		// is needed to draw the triangle
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Revert state 
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);

		//Disable active program
		glUseProgram(0);

		// Process all events on GLFW event queue 
		// Can be keyboard events, mouse or gamepad events
		glfwPollEvents();

		// Send framebuffer content of window to be draw on screen
		glfwSwapBuffers(Window);
	}

	// Unalocate VertexBuffer
	glDeleteBuffers(1, &VertexBuffer);

	// End GLFW
	glfwTerminate();

	
	return 0;
}