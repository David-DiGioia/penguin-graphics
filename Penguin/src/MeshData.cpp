#include "MeshData.h"

#include "Util.h"
#include "Constants.h"

namespace MeshData {

	Material::Material(const char* diffusePath, const char* specularPath)
		: m_diffuse{ diffusePath }, m_specular{ specularPath }, m_uniformBuffer{ 0 }
	{
		block.useSpecMap = (specularPath != DEFAULT_TEX);
		glGenBuffers(1, &m_uniformBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(block), &block, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	Material::~Material()
	{
		glDeleteBuffers(1, &m_uniformBuffer);
	}

	void Material::bind()
	{
		m_diffuse.bind(TextureBindings::DIFFUSE);
		m_specular.bind(TextureBindings::SPECULAR);
		glBindBufferBase(GL_UNIFORM_BUFFER, UboBindings::MATERIAL, m_uniformBuffer);
	}

	void Material::unbind()
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, UboBindings::MATERIAL, 0);
	}

	void Material::updateBuffer()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
		void* bufferPtr{ glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE) };
		memcpy(bufferPtr, &block, sizeof(block));
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}


	Model::Model(const char* objPath, const char* texturePath, const char* specularPath)
		: mesh{ Util::loadOBJ(objPath) }
		, material{ texturePath, specularPath }
	{
	}

}

// I used a pointer for modelVec so it's more obvious it's being modified
Object::Object(std::vector<MeshData::Model>* modelVec, const char* objPath,
	const char* texturePath, const char* specularPath)
	: m_index{ modelVec->size() }
	, m_modelVec{ *modelVec }
{
	modelVec->emplace_back(objPath, texturePath, specularPath);
}

MeshData::Model& Object::get()
{
	return m_modelVec[m_index];
}
