#include <iostream>

#include "main.h"


static bool has_focus = true;

void on_refresh(GLFWwindow* window) {
	display();
	glfwSwapBuffers(window);
}

void on_focus(GLFWwindow* window, int got_focus) {
	has_focus = got_focus;
}

int main(int argc, char** argv) {
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	std::cout << glGetString(GL_RENDERER) << "\n" << glGetString(GL_VERSION) << "\n" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n" << std::endl;

	glfwSwapInterval(1);
	glfwSetWindowRefreshCallback(window, on_refresh);
	glfwSetWindowFocusCallback(window, on_focus);

	init();

	while (!glfwWindowShouldClose(window)) {
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();

		// throttle if the window is not visible etc.
		while (!has_focus) {
			glfwWaitEvents();
		}
	}

	glfwTerminate();
	return 0;
}
