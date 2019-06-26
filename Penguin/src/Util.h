#pragma once

#include <string>

#include "Core.h"

namespace Util {

	std::string stringFromFile(const char* filePath);

	GLuint compileShader(GLenum type, const char* srcPath);

	GLuint generateProgram(const char* vertShaderPath, const char* fragShaderPath);

}