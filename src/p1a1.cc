#include "impl.h"


const char vs[] = R"(
#version 330 core

in vec4 vPosition;

void main() {
	gl_Position = vPosition;
}
)";


const char fs[] = R"(
#version 330 core

out vec4 fColor;

void main() {
	if(gl_PrimitiveID == 0) {
		fColor = vec4(0.0, 0.0, 1.0, 1.0);
	} else {
		fColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
}
)";


enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
const GLuint NumVertices = 6;


IMPL(p1a1) {
	gl_service service;

	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
	GLfloat vertices[NumVertices][2] = {
		{ -0.90, -0.90}, {0.85, -0.90}, { -0.90, 0.85}, {0.90, -0.85}, {0.90, 0.90}, { -0.85, 0.90}
	};
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	GLuint program = service.program_from_source(vs, fs);
	glUseProgram(program);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(vPosition);

	service.on_display([]() {
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(VAOs[Triangles]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	});

	service.run();
}