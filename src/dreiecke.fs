#version 330 core

out vec4 fColor;

void main() {
	if(gl_PrimitiveID == 0) {
		fColor = vec4(0.0, 0.0, 1.0, 1.0);
	} else {
		fColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
}
