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

	std::string VertexShaderSource = ReadFile("shaders/triangle_vert.glsl");

	std::cout << VertexShaderSource << std::endl;

	//Define a triangle in normalized coordinates
	std::array<glm::vec3, 3> Triangle = {
		glm::vec3{ -1.0f, -1.0f, 0.0f },
		glm::vec3{ 1.0f, -1.0f, 0.0f },
		glm::vec3{ 0.0f, 1.0f, 0.0f }
	};

	//Model Matrix
	glm::mat4 ModelMatrix = glm::identity<glm::mat4>();

	//Viiew Matrix
	glm::vec3 Eye{ 0, 0, 5 };
	glm::vec3 Center{ 0, 1, 0 };
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