#pragma once

#include <memory>
#include <vector>
#include "glm/glm.hpp"

#include "../MeshData.h"

namespace Scenes {

	struct SceneTest
	{
		const int MAX_MODELS{ 16 };
		const glm::vec4 CLEAR_COLOR{ 0.2f, 0.2f, 0.2f, 1.0f };
		std::vector<MeshData::Model> models;
		std::unique_ptr<MeshData::Camera> activeCamera;

		std::unique_ptr<Object> penguin;
		std::unique_ptr<Object> bulldozer;
		std::unique_ptr<Object> igloo;

		void init();
		void update(float delta);
		void gui();
		void keyEvent(int key, int scancode, int action, int mods);
	};

}