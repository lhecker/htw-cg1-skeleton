#include "gl_service.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>


gl_service::gl_service() : _has_focus(true) {
	if (glfwInit() != GL_TRUE) {
		throw std::runtime_error("glfwInit() != GL_TRUE");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	this->_window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

	if (!this->_window) {
		glfwTerminate();
		throw std::runtime_error("glfwCreateWindow() returned nullptr");
	}

	glfwSetWindowUserPointer(this->_window, this);
	glfwMakeContextCurrent(this->_window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("glewInit() != GLEW_OK");
	}
}

gl_service::~gl_service() {
}

void gl_service::run() {
	glfwSwapInterval(1);

	glfwSetWindowRefreshCallback(this->_window, [](GLFWwindow* window) {
		auto self = reinterpret_cast<gl_service*>(glfwGetWindowUserPointer(window));
		self->emit_display_s();
		glfwSwapBuffers(window);
	});

	glfwSetWindowFocusCallback(this->_window, [](GLFWwindow* window, int got_focus) {
		auto self = reinterpret_cast<gl_service*>(glfwGetWindowUserPointer(window));
		self->_has_focus = got_focus;
	});

	glfwSetKeyCallback(this->_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto self = reinterpret_cast<gl_service*>(glfwGetWindowUserPointer(window));

		switch (action) {
		case GLFW_RELEASE:
			self->emit_keyup_s(key);
			break;
		case GLFW_PRESS:
			self->emit_keydown_s(key);
			break;
		}
	});

	while (!glfwWindowShouldClose(this->_window)) {
		this->emit_display();
		glfwSwapBuffers(this->_window);
		glfwPollEvents();

		// throttle if the window is not visible etc.
		while (!_has_focus) {
			glfwWaitEvents();
		}
	}

	glfwTerminate();
}

// vs = vertex shader | fs = fragment shader
GLuint gl_service::program_from_file(const std::string& vsPath, const std::string& fsPath) {
	auto loadSourceFromFile = [](const std::string& path) -> std::string {
		std::ifstream fd(path, std::ios::binary);

		if (!fd) {
			throw std::runtime_error("Failed to open file!");
		}

		fd.seekg(0, std::ios::end);
		size_t len = fd.tellg();
		fd.seekg(0);

		if (len > std::numeric_limits<GLint>::max()) {
			throw std::runtime_error("Shader file too large!");
		}

		std::string str(len, '\0');

		if (str.size() < len) {
			throw std::bad_alloc();
		}

		fd.read((char*)str.data(), str.size());

		return str;
	};

	std::string vsSource = loadSourceFromFile(vsPath);
	std::string fsSource = loadSourceFromFile(fsPath);
	return this->program_from_source(vsSource, fsSource);
}

GLuint gl_service::program_from_source(const std::string& vsSource, const std::string& fsSource) {
	auto loadShaderFromFile = [](GLenum shaderType, const std::string& source) -> GLuint {
		GLuint shaderId = glCreateShader(shaderType);
		const GLchar* base = source.data();
		GLint len = source.length();
		glShaderSource(shaderId, 1, &base, &len);
		glCompileShader(shaderId);

		GLint result;

		/*
		 * Returns GL_TRUE if the last compile operation on shader
		 * was successful and GL_FALSE otherwise.
		 */
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE) {
			throw std::runtime_error("Compilation failed!");
		}

		/*
		 * Returns the number of characters in the information log for
		 * shader including the null termination character,
		 * otherwise 0.
		 */
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &result);

		if (result > 0) {
			std::string errormsg('\0', result - 1);
			glGetShaderInfoLog(shaderId, (GLuint)errormsg.size(), NULL, (char*)errormsg.data());
			throw std::runtime_error(errormsg);
		}

		return shaderId;
	};

	auto linkShaders = [](GLuint vsId, GLuint fsId) -> GLuint {
		GLuint programId = glCreateProgram();
		glAttachShader(programId, vsId);
		glAttachShader(programId, fsId);
		glLinkProgram(programId);

		// same as in loadShaderFromFile()
		GLint result;

		glGetProgramiv(programId, GL_LINK_STATUS, &result);

		if (result == GL_FALSE) {
			throw std::runtime_error("Linking failed!");
		}

		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &result);

		if (result > 0) {
			std::string errormsg('\0', result - 1);
			glGetProgramInfoLog(programId, (GLuint)errormsg.size(), NULL, (char*)errormsg.data());
			throw std::runtime_error(errormsg);
		}

		glDeleteShader(vsId);
		glDeleteShader(fsId);

		return programId;
	};


	GLuint vsId = loadShaderFromFile(GL_VERTEX_SHADER, vsSource);
	GLuint fsId = loadShaderFromFile(GL_FRAGMENT_SHADER, fsSource);
	return linkShaders(vsId, fsId);
}
