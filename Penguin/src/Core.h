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

#define SUPPRESS_NOTIFICATION
#define DEBUG_BREAK __debugbreak()

#ifdef _DEBUG
	#define DEBUG
#endif

#ifdef DEBUG
	#define ASSERT(x) if (!(x)) DEBUG_BREAK;
#else
	#define ASSERT(x)
#endif