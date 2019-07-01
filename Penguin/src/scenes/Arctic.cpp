#include "Arctic.h"

#include <numeric>
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "../Constants.h"

namespace Scenes {

	namespace {

		// input
		bool rightPressed{ false };
		bool leftPressed{ false };
		bool upPressed{ false };
		bool downPressed{ false };

		// camera
		glm::vec3 cameraTarget{ 0.0f, 0.0f, 0.0f };
		glm::vec3 cameraRelativePos{ 0.0f, 1.0f, 3.0f };
		float maxDistFromTarget{ 1.0f };
		float maxDistFromTargetSquared{ maxDistFromTarget * maxDistFromTarget };
		float cameraRotation{ 0.0f };

		// player
		float speed{ 5.0f };

		// constants
		const glm::vec3 upVec{ 0.0f, 1.0f, 0.0f };

		// debug
#ifdef DEBUG
		float guiDelta{ 0.0f };
		float guiCameraRotation{ 0.0f };
		constexpr size_t NANO_VEC_LEN{ 100 };
		std::vector<long long> nanoVec;
		unsigned long long nanoVecIndex{ 0 };
#endif
	}

	void Arctic::init()
	{
#ifdef DEBUG
		nanoVec.resize(NANO_VEC_LEN);
#endif

		activeCamera = std::make_unique<MeshData::Camera>();
		activeCamera->frustum.f = 100.0f;

		penguin = std::make_unique<Object>(&models, "data/mesh/penguin.obj", "data/textures/penguin_col.png");
		arctic = std::make_unique<Object>(&models, "data/mesh/arctic.obj", "data/textures/arctic_col.png");
		igloo = std::make_unique<Object>(&models, "data/mesh/igloo.obj", "data/textures/igloo_col.png");
		pole = std::make_unique<Object>(&models, "data/mesh/pole.obj", "data/textures/pole_col.png");
		bulldozer = std::make_unique<Object>(&models, "data/mesh/bulldozer.obj", "data/textures/bulldozer_col.png");

		activeCamera->transform.pos = glm::vec3{ 0.0f, 1.0f, 5.0f };
		arctic->get().transform.pos.z = -10.0f;
		igloo->get().transform.pos = glm::vec3{ 4.0f, 0.0f, -1.0f };
		pole->get().transform.pos = glm::vec3{ 1.0f, 0.0f, -2.0f };
		bulldozer->get().transform.pos = glm::vec3{ -8.0f, 0.0f, -4.0f };

		bulldozer->get().transform.rot = glm::angleAxis(-Constants::PI / 16.0f, upVec);
		igloo->get().transform.rot = glm::angleAxis(-Constants::PI / 3.0f, upVec);
	}

	// Input vec must be normalized!
	glm::fquat Arctic::quatFromDirection(const glm::vec3& direction)
	{
		float theta{ glm::orientedAngle(glm::vec2{direction.x, direction.z}, glm::vec2{0.0f, 1.0f}) };

		return glm::angleAxis(theta, upVec);
	}

	void Arctic::penguinInput(float delta)
	{
		glm::vec3 direction{ 0.0f, 0.0f, 0.0f };

		if (rightPressed)
			direction.x += 1.0f;
		if (leftPressed)
			direction.x -= 1.0f;
		if (upPressed)
			direction.z -= 1.0f;
		if (downPressed)
			direction.z += 1.0f;

		if (glm::length2(direction) > 0.001f)
		{
			direction = glm::angleAxis(cameraRotation, upVec) * direction;

			direction = glm::normalize(direction);
			penguin->get().transform.pos += speed * delta * direction;
			penguin->get().transform.rot = quatFromDirection(direction);
		}
	}

	void Arctic::updateCamera()
	{
#ifdef DEBUG
		cameraRotation = guiCameraRotation;
#endif
		glm::vec3 cameraRelativePosRotated = glm::angleAxis(cameraRotation, upVec) * cameraRelativePos;
		activeCamera->transform.pos = cameraTarget + cameraRelativePosRotated;

		activeCamera->transform.rot = glm::angleAxis(cameraRotation, upVec);
	}

	void Arctic::update(float delta)
	{
		penguinInput(delta);

		if (glm::distance2(penguin->get().transform.pos, cameraTarget) > maxDistFromTargetSquared)
		{
			glm::vec3 p{ penguin->get().transform.pos };
			cameraTarget = glm::normalize(cameraTarget - p) * maxDistFromTarget + p;
		}

		updateCamera();

#ifdef DEBUG
		guiDelta = delta;
		nanoVec[nanoVecIndex++ % NANO_VEC_LEN] = renderTimeNano;
#endif
	}

	void Arctic::gui()
	{
#ifdef DEBUG
		ImGui::Begin("Debug");

		ImGui::SliderFloat("Camera Angle", &guiCameraRotation, 0.0f, 2.0f * Constants::PI);

		long long nanoAvg{ std::accumulate(nanoVec.begin(), nanoVec.end(), 0) / NANO_VEC_LEN };

		ImGui::Text("Render time: %.3f avg ms", static_cast<double>(nanoAvg) / 1000000);
		ImGui::Text("Delta time: %.3f s", guiDelta);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
#endif
	}

	void Arctic::keyEvent(int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_D:
				rightPressed = true;
				break;
			case GLFW_KEY_A:
				leftPressed = true;
				break;
			case GLFW_KEY_W:
				upPressed = true;
				break;
			case GLFW_KEY_S:
				downPressed = true;
				break;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (key)
			{
			case GLFW_KEY_D:
				rightPressed = false;
				break;
			case GLFW_KEY_A:
				leftPressed = false;
				break;
			case GLFW_KEY_W:
				upPressed = false;
				break;
			case GLFW_KEY_S:
				downPressed = false;
				break;
			}
		}
	}

}