#pragma once

#include <memory>
#include <vector>
#include "glm/glm.hpp"

#include "../MeshData.h"

namespace Scenes {

	struct Arctic
	{
		const int MAX_MODELS{ 16 };
		const glm::vec4 CLEAR_COLOR{ 0.0f, 0.5f, 0.8f, 1.0f };
		std::vector<MeshData::Model> models;
		std::unique_ptr<MeshData::Camera> activeCamera;
		void init();
		void update(float delta);
		void gui();
		void keyEvent(int key, int scancode, int action, int mods);
		// -----------------------------------------------------------

		// models
		std::unique_ptr<Object> penguin;
		std::unique_ptr<Object> arctic;
		std::unique_ptr<Object> igloo;
		std::unique_ptr<Object> pole;
		std::unique_ptr<Object> bulldozer;

		// input
		bool rightPressed{ false };
		bool leftPressed{ false };
		bool upPressed{ false };
		bool downPressed{ false };

		// player
		float speed{ 1.0f };

		// camera
		glm::vec3 cameraTarget{ 0.0f, 0.0f, 0.0f };
		glm::vec3 cameraRelativePos{ 0.0f, 1.0f, 3.0f };
		float maxDistFromTarget{ 1.0f };
		float maxDistFromTargetSquared{ maxDistFromTarget * maxDistFromTarget };

		glm::fquat quatFromDirection(const glm::vec3& direction);
		void penguinInput(float delta);
		void updateCamera();
	};

}
