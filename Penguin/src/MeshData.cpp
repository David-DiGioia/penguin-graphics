#include "MeshData.h"

#include "Util.h"

Model::Model(const char* objPath, const char* texturePath)
	: mesh{ Util::loadOBJ(objPath) }
	, colorMap{ texturePath }
{
}