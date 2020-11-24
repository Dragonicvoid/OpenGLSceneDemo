#include "Window.h"

Window::Window()
{
	width = 1366;
	height = 768;

	for (size_t i = 0; i < 1024; i++)
		keys[i] = 0;
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++)
		keys[i] = 0;
}

int Window::Initialise()
{
	if (!glfwInit()) 
	{
		printf("Error Initialising GLFW\n");
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Main Window", NULL, NULL);
	if (!mainWindow) 
	{
		printf("Error Initialising Window\n");
		glfwTerminate();
		return 1;
	}

	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);

	//Handling a keyboard event
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// allow modern extension access
	glewExperimental = GL_TRUE;

	GLenum error = glewInit();
	if (error != GLEW_OK) 
	{
		printf("Error: %s\n", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// Create Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
}

void Window::handleKeys(GLFWwindow * window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			theWindow->keys[key] = true;
		else if (action == GLFW_RELEASE)
			theWindow->keys[key] = false;
	}
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
