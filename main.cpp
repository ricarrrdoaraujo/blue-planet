#include <iostream>
#include <cassert>
#include <array>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

const int Width = 800;
const int Height = 600;

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

	//Define a triangle in normalized coordinates
	std::array<glm::vec3, 3> Triangle = {
		glm::vec3{ -1.0f, -1.0f, 0.0f },
		glm::vec3{ 1.0f, -1.0f, 0.0f },
		glm::vec3{ 0.0f, 1.0f, 0.0f }
	};

	//Copy triangle vertices to GPU memory
	GLuint VertexBuffer;

	// Ask OpenGL generate an identifier of VertexBuffer
	glGenBuffers(1, &VertexBuffer);

	// Activate VertexBuffer as the buffer where we copy the triangle data to
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	//Copy data do video memory
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

		// Inform to opengl where in the VertexBuffer are the vertices 
		// In the case of array Triangles is contiguous in memory, is just necessary to inform how much vertices 
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