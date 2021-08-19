#include <iostream>
#include <cassert>

#include <GLFW/glfw3.h>

const int Width = 800;
const int Height = 600;

int main()
{
	// initialize GLFW
	assert(glfwInit());

	// Create window
	GLFWwindow* Window = glfwCreateWindow(Width, Height, "Blue Planet", nullptr, nullptr);
	assert(Window);

	// Start event loop
	while (!glfwWindowShouldClose(Window))
	{
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