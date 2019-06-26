#include "Util.h"

#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>

namespace Util {

	std::string stringFromFile(const char* filePath)
	{
		std::ifstream stream{ filePath };
		ASSERT(stream);

		std::stringstream buffer;
		buffer << stream.rdbuf();
		return buffer.str();
	}

	GLuint compileShader(GLenum type, const char* srcPath)
	{
		std::string srcString{ stringFromFile(srcPath) };
		const char* src{ srcString.c_str() };

		GLuint shader{ glCreateShader(type) };
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		// Error Checking
		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength{ 0 };
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

			const char* typeString{ (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT" };

			std::cerr << "ERROR: " << typeString << " SHADER FAILED COMPILATION.\n" << &errorLog[0] << '\n';

			glDeleteShader(shader); // Don't leak the shader.
			DEBUG_BREAK;
		}

		return shader;
	}

	GLuint generateProgram(const char* vertShaderPath, const char* fragShaderPath)
	{
		GLuint vertShader{ compileShader(GL_VERTEX_SHADER, vertShaderPath) };
		GLuint fragShader{ compileShader(GL_FRAGMENT_SHADER, fragShaderPath) };

		GLuint program{ glCreateProgram() };
		glAttachShader(program, vertShader);
		glAttachShader(program, fragShader);
		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)& isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);

			// Use the infoLog as you see fit.
			std::cerr << "Linking program failed.\n";
		}

		glDetachShader(program, vertShader);
		glDetachShader(program, fragShader);

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		return program;
	}

}