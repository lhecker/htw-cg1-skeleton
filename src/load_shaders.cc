#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

#include "load_shaders.h"


namespace {
struct array_deleter {
	void operator()(const char* base) const {
		delete[] base;
	}
};
}


// vs = vertex shader | fs = fragment shader
GLuint load_shaders(const std::string& vsPath, const std::string& fsPath) {
	auto loadShaderFromFile = [](GLenum shaderType, const std::string& path) -> GLuint {
		std::cout << "- " << path << std::endl;

		try {
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

			// using std::unique_ptr for exception safety
			char* base = new char[len];
			std::unique_ptr<char, array_deleter> base_owner(base, array_deleter());

			fd.read(base, len);

			GLuint shaderId = glCreateShader(shaderType);
			glShaderSource(shaderId, 1, &base, (GLint*)&len);
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
		} catch (const std::runtime_error& e) {
			std::cout << "    " << e.what() << std::endl;
		}

		abort();
	};


	auto linkShaders = [](GLuint vsId, GLuint fsId) -> GLuint {
		std::cout << "+ linking" << std::endl;

		try {
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
		} catch (const std::runtime_error& e) {
			std::cout << "    " << e.what() << std::endl;
		}

		abort();
	};


	std::cout << "compiling shader" << std::endl;
	GLuint vsId = loadShaderFromFile(GL_VERTEX_SHADER, vsPath);
	GLuint fsId = loadShaderFromFile(GL_FRAGMENT_SHADER, fsPath);
	return linkShaders(vsId, fsId);
}