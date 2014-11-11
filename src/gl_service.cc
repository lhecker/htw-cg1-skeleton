#include "gl_service.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>


static std::string get_log(GLuint object) {
	GLint log_length = 0;

	if (glIsShader(object)) {
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else if (glIsProgram(object)) {
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else {
		return "Not a shader, nor a program!";
	}

	std::string log(log_length, ' ');

	if (glIsShader(object)) {
		glGetShaderInfoLog(object, log.size(), NULL, (char*)log.data());
	} else {
		glGetProgramInfoLog(object, log.size(), NULL, (char*)log.data());
	}

	return log;
}


gl_service::gl_service(const std::string& title) : _has_focus(true) {
	if (glfwInit() != GL_TRUE) {
		throw std::runtime_error("glfwInit() != GL_TRUE");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	this->_window = glfwCreateWindow(640, 480, title.c_str(), nullptr, nullptr);

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

	glfwSetWindowFocusCallback(this->_window, [](GLFWwindow* window, int got_focus) {
		auto self = reinterpret_cast<gl_service*>(glfwGetWindowUserPointer(window));
		self->_has_focus = got_focus;
		printf("glfwSetWindowFocusCallback\n");
	});

	glfwSetFramebufferSizeCallback(this->_window, [](GLFWwindow* window, int width, int height) {
		auto self = reinterpret_cast<gl_service*>(glfwGetWindowUserPointer(window));
		glViewport(0, 0, width, height);
		self->emit_reshape_s(width, height);
		printf("glfwSetFramebufferSizeCallback\n");
	});

	glfwSetWindowRefreshCallback(this->_window, [](GLFWwindow* window) {
		auto self = reinterpret_cast<gl_service*>(glfwGetWindowUserPointer(window));

		double t = glfwGetTime();
		self->emit_display_s(t - self->_time);
		self->_time = t;

		glfwSwapBuffers(window);
	});

	glfwSetKeyCallback(this->_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto self = reinterpret_cast<gl_service*>(glfwGetWindowUserPointer(window));

		switch (action) {
		case GLFW_RELEASE:
			if (key == GLFW_KEY_ESCAPE && self->_curser_disabled) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			self->emit_keyup_s(key);
			break;
		case GLFW_PRESS:
			self->emit_keydown_s(key);
			break;
		}
	});

	glfwSetMouseButtonCallback(this->_window, [](GLFWwindow* window, int button, int action, int mods) {
		auto self = reinterpret_cast<gl_service*>(glfwGetWindowUserPointer(window));

		switch (action) {
			case GLFW_RELEASE:
				if (self->_curser_disabled) {
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}

				self->emit_mouseup_s(button);
				break;
			case GLFW_PRESS:
				self->emit_mousedown_s(button);
				break;
		}
	});

	glfwSetCursorPosCallback(this->_window, [](GLFWwindow* window, double xpos, double ypos) {
		auto self = reinterpret_cast<gl_service*>(glfwGetWindowUserPointer(window));

		if (self->_has_focus && (!self->_curser_disabled || glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)) {
			self->emit_mousemoved_s(xpos, ypos);
		}
	});

	glfwSetScrollCallback(this->_window, [](GLFWwindow* window, double xoffset, double yoffset) {
		auto self = reinterpret_cast<gl_service*>(glfwGetWindowUserPointer(window));
		self->emit_scroll_s(xoffset, yoffset);
	});

	{
		int width;
		int height;
		glfwGetFramebufferSize(this->_window, &width, &height);
		this->emit_reshape_s(width, height);
	}

	this->_time = glfwGetTime();

	while (!glfwWindowShouldClose(this->_window)) {
		double t = glfwGetTime();
		this->emit_display_s(t - this->_time);
		this->_time = t;

		glfwSwapBuffers(this->_window);

		glfwPollEvents();

		// throttle if the window is not visible etc.
		while (!_has_focus) {
			glfwWaitEvents();
		}
	}

	glfwTerminate();
}

double gl_service::time() const {
	return this->_time;
}

void gl_service::set_cursor_disabled(bool disabled) {
	this->_curser_disabled = disabled;

	if (disabled && this->_has_focus) {
		glfwSetInputMode(this->_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void gl_service::set_cursor_position(double xpos, double ypos) {
	glfwSetCursorPos(this->_window, xpos, ypos);
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
	return gl_service::program_from_source(vsSource, fsSource);
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
			throw std::runtime_error(get_log(shaderId));
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
			throw std::runtime_error(get_log(programId));
		}

		glDeleteShader(vsId);
		glDeleteShader(fsId);

		return programId;
	};


	GLuint vsId = loadShaderFromFile(GL_VERTEX_SHADER, vsSource);
	GLuint fsId = loadShaderFromFile(GL_FRAGMENT_SHADER, fsSource);
	return linkShaders(vsId, fsId);
}
