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
		glm::vec4 CLEAR_COLOR{ 0.0f, 0.5f, 0.8f, 1.0f };
		std::vector<MeshData::Model> models;
		std::unique_ptr<MeshData::Camera> activeCamera;
		void updateLighting(float time);
		void updateLightBuffer();
		void transformPointLights(const glm::mat4& worldToCamera);
		void initLights(unsigned int program);
		void init();
		void update(float delta);
		void gui();
		void keyEvent(int key, int scancode, int action, int mods);
		// -----------------------------------------------------------

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
