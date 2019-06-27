#include "MeshData.h"

#include "Util.h"

Model::Model(const char* objPath, const char* texturePath)
	: mesh{ Util::loadOBJ(objPath) }
	, colorMap{ texturePath }
{
}

// I used a pointer for modelVec so it's more obvious it's being modified
Object::Object(std::vector<Model>* modelVec, const char* objPath, const char* texturePath)
	: m_index{ modelVec->size() }
	, m_modelVec{ *modelVec }
{
	modelVec->emplace_back(objPath, texturePath);
}

Model& Object::get()
{
	return m_modelVec[m_index];
}