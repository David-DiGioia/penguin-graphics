#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

#define DEBUG_BREAK __debugbreak()

#define ASSERT(x) if (!(x)) DEBUG_BREAK;