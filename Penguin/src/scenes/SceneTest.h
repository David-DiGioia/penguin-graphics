#pragma once

#include <memory>
#include <vector>
#include "glm/glm.hpp"

#include "../MeshData.h"

namespace Scenes {

	// Scene MUST implement:
	// update()
	// init()
	// gui()

	// Scene MUST contain:
	// const int MAX_MODELS
	// const glm::vec4 CLEAR_COLOR
	// std::vector<MeshData::Model> models

	struct SceneTest
	{
		const int MAX_MODELS{ 16 };
		const glm::vec4 CLEAR_COLOR{ 1.0f, 0.0f, 0.0f, 1.0f };
		std::vector<MeshData::Model> models;


		std::unique_ptr<MeshData::Camera> activeCamera;
		std::unique_ptr<Object> penguin;
		std::unique_ptr<Object> bulldozer;
		std::unique_ptr<Object> igloo;

		void init();
		void update();
		void gui();
	};

}