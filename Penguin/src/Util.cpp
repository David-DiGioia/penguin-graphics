#include "Util.h"

#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>

namespace Util {

	glm::mat4 createProjMatrix(const FrustumData& fd)
	{
		return glm::mat4{
			(2 * fd.p) / (fd.r - fd.l), 0.0f, 0.0f, 0.0f,
			0.0f, (2 * fd.p) / (fd.t - fd.b), 0.0f, 0.0f,
			(fd.r + fd.l) / (fd.r - fd.l), (fd.t + fd.b) /
			(fd.t - fd.b), (fd.n + fd.f) / (fd.n - fd.f), -1.0f,
			0.0f, 0.0f, (2 * fd.f * fd.n) / (fd.n - fd.f), 0.0f
		};
	}

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

}