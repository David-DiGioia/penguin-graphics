#pragma once

#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "Texture.h"

namespace MeshData {

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

	class Material
	{
	public:
		Material(const char* texPath);
		~Material();
		void bind();
		void unbind();

		// Maps to glsl uniform block
		// No texture because sampler2D is an opaque type and can't be in uniform block
		struct Block
		{
			glm::vec4 specularColor{ 1.0f, 1.0f, 1.0f, 1.0f };
			float specularShininess{ 0.5f };
		} block;

	private:
		Texture m_diffuse;
		unsigned int m_uniformBuffer;
	};

	struct Model
	{
		Mesh mesh;
		Material material;
		Transform transform;

		Model(const char* objPath, const char* texturePath = DEFAULT_TEX);
	};

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

	struct Camera
	{
		Transform transform;
		FrustumData frustum;
	};

}

class Object
{
public:
	Object(std::vector<MeshData::Model>* modelVec, const char* objPath, const char* texturePath = MeshData::DEFAULT_TEX);
	MeshData::Model& get();

private:
	unsigned long long m_index;
	std::vector<MeshData::Model>& m_modelVec;
};