#pragma once

#include <memory>
#include <vector>
#include "glm/glm.hpp"

#include "../Core.h"
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

		glm::vec4 dirToLight{ 0.866f, 0.5f, 0.0f, 0.0f };
		glm::vec4 lightIntensity{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 ambientLightIntensity{ 0.5f, 0.5f, 0.5f, 1.0f };

		glm::vec3 pointLightPos{ 0.0f, 1.5f, 0.0f };
		
#ifdef DEBUG
		long long renderTimeNano{ 0 };
#endif

		// models
		std::unique_ptr<Object> penguin;
		std::unique_ptr<Object> arctic;
		std::unique_ptr<Object> igloo;
		std::unique_ptr<Object> pole;
		std::unique_ptr<Object> bulldozer;

		glm::fquat quatFromDirection(const glm::vec3& direction);
		void penguinInput(float delta);
		void updateCamera();
	};

}
