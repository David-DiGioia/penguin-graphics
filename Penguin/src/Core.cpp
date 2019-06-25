#include "Core.h"

#include <cstdio>

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	const char* severityString;
	const char* typeString;
	bool debugBreak{ false };

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		severityString = "HIGH";
		debugBreak = true;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		severityString = "MEDIUM";
		debugBreak = true;
		break;
	case GL_DEBUG_SEVERITY_LOW:
		severityString = "LOW";
		debugBreak = true;
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		severityString = "NOTIFICATION";
		break;
	default:
		severityString = "[unknown]";
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		typeString = "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		typeString = "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		typeString = "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		typeString = "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		typeString = "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_MARKER:
		typeString = "MARKER";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		typeString = "PUSH_GROUP";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		typeString = "POP_GROUP";
		break;
	case GL_DEBUG_TYPE_OTHER:
		typeString = "OTHER";
		break;
	default:
		typeString = "[unknown]";
	}

	fprintf(stderr, "%s\nGL CALLBACK: type = %s 0x%x, severity = %s 0x%x\n%s\n\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		typeString, type, severityString, severity, message);

	if (debugBreak)
		DEBUG_BREAK;
}
