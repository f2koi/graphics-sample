#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <ctime>
#include <cmath>
#include <iostream>
#include <chrono>
#include <vector>
#include "shader.h"

using namespace std;

float translate[3] = { 0 };
constexpr float TRANSLATE_UNIT = 0.1;

float vertices_1[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f,
};
float vertices_2[] = {
	-0.7f, -0.7f, 0.0f,
	 0.9f, -0.7f, 0.0f,
	 0.9f,  0.0f, 0.0f,
};

GLuint vb_triangles[2];
GLuint vertex_array;
GLuint vshader, fshader;
GLuint program;

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	glBindBuffer(GL_ARRAY_BUFFER, vb_triangles[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glm::mat4 model_view_matrix = glm::translate(glm::vec3(translate[0], translate[1], translate[2]));
	glUniformMatrix4fv(1, 1, GL_FALSE, &model_view_matrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindBuffer(GL_ARRAY_BUFFER, vb_triangles[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glUniformMatrix4fv(1, 1, GL_FALSE, &glm::identity<glm::mat4>()[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glutPostRedisplay();
	glutSwapBuffers();
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
}

void special_keyboard(int key, int width, int height) {
	switch (key) {
	case GLUT_KEY_UP:
		translate[1] += TRANSLATE_UNIT;
		break;
	case GLUT_KEY_LEFT:
		translate[0] -= TRANSLATE_UNIT;
		break;
	case GLUT_KEY_RIGHT:
		translate[0] += TRANSLATE_UNIT;
		break;
	case GLUT_KEY_DOWN:
		translate[1] -= TRANSLATE_UNIT;
		break;
	default:
		break;
	}
}

void idle() {
	auto clock = std::chrono::high_resolution_clock();
	static auto time_previous = clock.now();
	unsigned long time_delta_micros = (clock.now() - time_previous).count() / 1000;
	time_previous = clock.now();
}

// https://www.geeksforgeeks.org/rendering-triangle-using-openglusing-shaders/
GLuint compile_shader(const string source, GLenum type) {
	const char* shader_source = source.c_str();
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (!compileStatus) {
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* error_msg = new char[length];
		glGetShaderInfoLog(shader, length, &length, error_msg);
		cout << "Cannot Compile Shader: " << error_msg << endl;
		delete[] error_msg;
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

void setup_vertex_buffers_and_arrays() {
	glCreateBuffers(2, &vb_triangles[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vb_triangles[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1), vertices_1, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vb_triangles[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_STATIC_DRAW);

	glCreateVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
}

void setup_shader() {
	const string vshader_source = load_shader_source("vshader.glsl");
	const string fshader_source = load_shader_source("fshader.glsl");
	vshader = compile_shader(vshader_source, GL_VERTEX_SHADER);
	fshader = compile_shader(fshader_source, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	if (!program) {
		cout << "Error Creating Shader Program" << endl;
		return;
	}

	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);

	glDetachShader(program, vshader);
	glDetachShader(program, fshader);
	glDeleteShader(fshader);
	glDeleteShader(vshader);

	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus) {
		cout << "Error Linking program" << endl;
		glDeleteProgram(program);
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitContextVersion(4, 6);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("OpenGL window");

	GLenum glew_init_result = glewInit();
	if (glew_init_result != GLEW_OK) {
		cout << "GLEW init error: " << glewGetErrorString(glew_init_result) << endl;
		return -1;
	}
	cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	setup_shader();
	setup_vertex_buffers_and_arrays();

	glutInitWindowSize(1080, 720);
	glEnable(GL_DEPTH_TEST);

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutSpecialFunc(special_keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;
}
