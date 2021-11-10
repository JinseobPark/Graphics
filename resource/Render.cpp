/**
\file        Render.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        19/3/2018
\brief		 Render 2D line segments and triangles

*/
#include "Render.h"
#include <iostream>
#include <stdexcept>

using namespace std;

GLuint CompileShader(const char*& shader_text, GLenum shader_type)
{
	GLuint shader = glCreateShader(shader_type);
	GLint value = 0;
	glShaderSource(shader, 1, &shader_text, nullptr);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &value);
	if (value == GL_FALSE)
	{
		GLint logSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		runtime_error("Failed to compile");
		if (shader_type == GL_FRAGMENT_SHADER)
			runtime_error("fragment shader:");
		else if (shader_type == GL_VERTEX_SHADER)
			runtime_error("vertex shader:\n");
		assert(false);
	}
	return shader;
}

GLuint LinkShader(GLuint fragment_handle, GLuint shader_handle)
{
	GLint value = 0;
	auto program = glCreateProgram();
	glAttachShader(program, fragment_handle);
	glAttachShader(program, shader_handle);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &value);
	if (value == GL_FALSE)
	{
		runtime_error("shader program failed to link\n");
	}
	// No longer need shader objects after linking the shader program
	glDeleteShader(fragment_handle);
	glDeleteShader(shader_handle);
	return program;
}
Render::Render(void)
{
	const char* fragment_shader_text =
		R"(#version 130
  uniform vec3 color;
  out vec4 frag_color;
  void main(void) {
    frag_color = vec4(color,1);
  })";

	const char* vertex_shader_text =
		R"(#version 130
  attribute vec4 position;
    void main() {
    gl_Position = position;
  })";

	auto fshader = CompileShader(fragment_shader_text, GL_FRAGMENT_SHADER);
	auto vshader = CompileShader(vertex_shader_text, GL_VERTEX_SHADER);
	program = LinkShader(fshader, vshader);

	// enable z-buffer
	glEnable(GL_DEPTH_TEST);

	// get shader parameter handles
	aposition = glGetAttribLocation(program, "position");
	ucolor = glGetUniformLocation(program, "color");


}

Render::~Render(void)
{
	glUseProgram(0);
	glDeleteProgram(program);
}

void Render::ClearBuffers(const Vector & c)
{
	glClearColor(c.x, c.y, c.z, c.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::SetColor(const Vector & c)
{
	glColor4f(c.x, c.y, c.z, c.w);
}

void Render::DrawLine(const Hcoord & P, const Hcoord & Q)
{
	float face_vertices[8] =
	{
		P.x,P.y,P.z,P.w,
		Q.x,Q.y,Q.z,Q.w,
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, 0, face_vertices);
	glDrawArrays(GL_LINES, 0, 2);
}

void Render::FillTriangle(const Hcoord & P, const Hcoord & Q, const Hcoord & R)
{
	float face_vertices[12] =
	{
		P.x,P.y,P.z,P.w,
		Q.x,Q.y,Q.z,Q.w,
		R.x,R.y,R.z,R.w
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, 0, face_vertices);
	glDrawArrays(GL_TRIANGLES, 0, 3);

}
