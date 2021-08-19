#include <iostream>
#include <cassert>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

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

	//Define background color
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	// Start event loop
	while (!glfwWindowShouldClose(Window))
	{
		//Clear framebuffer. GL_COLOR_BUFFER_BIT clear color buffer and fullfil with the color defined on glClearColor
		glClear(GL_COLOR_BUFFER_BIT);

		// Process all events on GLFW event queue 
		// Can be keyboard events, mouse or gamepad events
		glfwPollEvents();

		// Send framebuffer content of window to be draw on screen
		glfwSwapBuffers(Window);
	}


	// End GLFW
	glfwTerminate();

	
	return 0;
}