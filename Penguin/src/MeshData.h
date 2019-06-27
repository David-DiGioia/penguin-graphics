#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "Texture.h"

constexpr const char* DEFAULT_TEX{ "data/textures/default.png" };

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

struct Transform
{
	glm::vec3 pos{ 0.0f, 0.0f, 0.0f };
	glm::fquat rot{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
};

struct Model
{
	Mesh mesh;
	Texture colorMap;
	Transform transform;

	Model(const char* objPath, const char* texturePath = DEFAULT_TEX);
};

class Object
{
public:
	Object(std::vector<Model>* modelVec, const char* objPath, const char* texturePath = DEFAULT_TEX);
	Model& get();

private:
	unsigned long long m_index;
	std::vector<Model>& m_modelVec;
};