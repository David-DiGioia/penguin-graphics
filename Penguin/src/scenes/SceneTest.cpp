#include "SceneTest.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Scenes {

	void SceneTest::init()
	{
		camera = std::make_unique<MeshData::Camera>();
		penguin = std::make_unique<Object>(&models, "data/mesh/penguin.obj", "data/textures/penguin_col.png");
		bulldozer = std::make_unique<Object>(&models, "data/mesh/bulldozer.obj", "data/textures/bulldozer_col.png");
		igloo = std::make_unique<Object>(&models, "data/mesh/igloo.obj", "data/textures/igloo_col.png");
	}

	glm::vec3 position{ 0.0f, 0.0f, -2.0f };
	float angle{ 0 };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

	glm::vec3 positionC{ 0.0f, 0.0f, 0.0f };
	float angleC{ 0 };

	void SceneTest::update()
	{
		glm::vec3 axis{ 0.0f, 1.0f, 0.0f };
		axis = glm::normalize(axis);
		glm::fquat orientation{ glm::angleAxis(angle, axis) };

		penguin->get().transform.pos = position;
		penguin->get().transform.rot = orientation;
		penguin->get().transform.scale = scale;

		glm::fquat orientationC{ glm::angleAxis(angleC, axis) };
		camera->transform.pos = positionC;
		camera->transform.rot = orientationC;
	}

	void SceneTest::gui()
	{

	}

}