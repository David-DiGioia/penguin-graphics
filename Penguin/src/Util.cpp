#include "Util.h"

#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>

namespace Util {

	glm::mat4 createProjMatrix(const MeshData::FrustumData& fd)
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

	std::vector<std::string> splitString(const std::string& s, const std::string& delim)
	{
		size_t beg{ 0 };
		size_t end{ 0 };
		std::vector<std::string> result;

		std::string token;
		while ((end = s.find(delim, beg)) != std::string::npos)
		{
			token = s.substr(beg, end - beg);
			result.push_back(token);
			beg = end + delim.length();
		}
		result.push_back(s.substr(beg, s.length() - beg));

		return result;
	}

	MeshData::Mesh loadOBJ(const char* path)
	{
		std::ifstream stream{ path };
		ASSERT(stream);

		constexpr int MAX_LINE_LENGTH{ 128 };

		MeshData::Mesh result;
		std::vector<glm::vec3> tempPositions;
		std::vector<glm::vec2> tempTexCoords;
		std::vector<glm::vec3> tempNormals;

		char currentLine[MAX_LINE_LENGTH];
		while (stream.getline(currentLine, MAX_LINE_LENGTH))
		{
			std::vector<std::string> strings{ splitString(std::string{currentLine}, std::string{" "}) };

			if (strings[0] == "v")
			{
				tempPositions.emplace_back(
					std::stof(strings[1]), std::stof(strings[2]), std::stof(strings[3])
				);
			}
			else if (strings[0] == "vt")
			{
				tempTexCoords.emplace_back(
					std::stof(strings[1]), std::stof(strings[2])
				);
			}
			else if (strings[0] == "vn")
			{
				tempNormals.emplace_back(
					std::stof(strings[1]), std::stof(strings[2]), std::stof(strings[3])
				);
			}
			else if (strings[0] == "f")
			{
				for (int i{ 1 }; i < 4; ++i)
				{
					std::vector<std::string> strIndices{ splitString(strings[i], std::string{"/"}) };

					// Subtract 1 since obj indices start at 1 instead of 0
					result.vertices.push_back(MeshData::Vertex{
						tempPositions[(long long int)std::stoi(strIndices[0]) - 1],
						tempTexCoords[(long long int)std::stoi(strIndices[1]) - 1],
						tempNormals[(long long int)std::stoi(strIndices[2]) - 1]
						});
				}
			}
		}
		return result;
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