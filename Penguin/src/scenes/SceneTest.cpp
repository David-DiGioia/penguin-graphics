#include "SceneTest.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "../Constants.h"

namespace Scenes {

	void SceneTest::init()
	{
		activeCamera = std::make_unique<MeshData::Camera>();
		penguin = std::make_unique<Object>(&models, "data/mesh/penguin.obj", "data/textures/penguin_col.png");
		bulldozer = std::make_unique<Object>(&models, "data/mesh/bulldozer.obj", "data/textures/bulldozer_col.png");
		igloo = std::make_unique<Object>(&models, "data/mesh/igloo.obj", "data/textures/igloo_col.png");
	}

	// Controlled by gui
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
		activeCamera->transform.pos = positionC;
		activeCamera->transform.rot = orientationC;
	}

	void SceneTest::gui()
	{
		ImGui::Begin("Debug");
		ImGui::Text("Model transform:");

		ImGui::SliderFloat("Angle", &angle, 0.0f, 2.0f * Constants::PI);
		ImGui::SliderFloat3("Position", &position.x, -3.0f, 3.0f);
		ImGui::SliderFloat3("Scale", &scale.x, 0.0f, 3.0f);

		ImGui::Text("Camera transform:");

		ImGui::SliderFloat("AngleC", &angleC, 0.0f, 2.0f * Constants::PI);
		ImGui::SliderFloat3("PositionC", &positionC.x, -3.0f, 3.0f);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

}