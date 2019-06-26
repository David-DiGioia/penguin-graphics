#pragma once

#include <string>
#include "glm/glm.hpp"

#include "Core.h"

namespace Util {

	struct FrustumData
	{
		// projection plane
		float p{ 1.0f };
		// near, far
		float n{ 0.5f }, f{ 10.0f };
		// left, right
		float l{ -1.0f }, r{ 1.0f };
		// top, bottom
		float t{ 1.0f }, b{ -1.0f };
	};

	glm::mat4 createProjMatrix(const FrustumData& fd);

	std::string stringFromFile(const char* filePath);

	GLuint compileShader(GLenum type, const char* srcPath);

}