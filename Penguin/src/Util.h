#pragma once

#include <string>
#include <chrono>
#include "glm/glm.hpp"

#include "Core.h"
#include "MeshData.h"

namespace Util {

	class Timer
	{
	public:
		Timer();
		long long getMilliseconds();
		long long getNanoseconds();

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_begin;
	};

	glm::mat4 createProjMatrix(const MeshData::FrustumData& fd);

	std::string stringFromFile(const char* filePath);

	std::vector<std::string> splitString(const std::string& s, const std::string& delim);

	MeshData::Mesh loadOBJ(const char* path);

	GLuint compileShader(GLenum type, const char* srcPath);

}