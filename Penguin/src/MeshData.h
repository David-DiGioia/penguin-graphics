#pragma once

#include <vector>
#include "glm/glm.hpp"

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	//std::vector<unsigned int> indices;
};