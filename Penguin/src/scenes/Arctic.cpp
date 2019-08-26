#include "Arctic.h"

#include <numeric>
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "../Constants.h"
#include "../Util.h"

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

		// lights
		const float nightMaxIntensity{ 0.5f };
		const float dayMaxIntensity{ 12.0f };

		const glm::vec4 nightAmbient{ 0.01f, 0.01f, 0.012f, 1.0f };
		const glm::vec4 dayAmbient{ 1.3f, 1.3f, 1.5f, 1.0f };

		glm::vec4 sunDirWorld{ 0.0f, -1.0f, 0.0f, 0.0f };
		float sunBaseIntensity{ 10.0f };
		glm::vec4 ambientIntensity{ 0.2f, 0.2f, 0.2f, 1.0f };
		float dayCycleTime{ 24.0f };
		Util::Gradient<glm::vec4> skyGradient{ glm::vec4{ 0.0f, 0.0f, 0.02f, 1.0f } };
		Util::Gradient<glm::vec4> sunGradient{ glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f } };
		Util::Gradient<glm::vec4> ambientGradient{ nightAmbient };
		Util::Gradient<float> maxGradient{ nightMaxIntensity };
		float timeOfDay{ 0.0f };

		constexpr static int NUMBER_OF_LIGHTS{ 2 };
		// subtract 1 since zeroth index is directional light
		// 0.0f, 1.5f, 0.0f
		glm::vec4 pointPos[NUMBER_OF_LIGHTS - 1]{ { 4.0f, 1.0f, -1.0f, 1.0f } };
		glm::vec4 pointIntensity[NUMBER_OF_LIGHTS - 1]{ {7.0f, 7.0f, 6.0f, 1.0f } };

		// constants
		const glm::vec3 vecY{ 0.0f, 1.0f, 0.0f };
		const glm::vec3 vecZ{ 0.0f, 0.0f, 1.0f };

		// debug
#ifdef DEBUG
		float guiDelta{ 0.0f };
		float guiCameraRotation{ 0.0f };
		float guiSpecular{ 0.15f };
		glm::vec3 guiSpecColor{ 0.38f, 0.38f, 0.7f };
		constexpr size_t NANO_VEC_LEN{ 100 };
		std::vector<long long> nanoVec;
		unsigned long long nanoVecIndex{ 0 };
#endif

		struct PerLight
		{
			glm::vec4 cameraSpaceLightPos;
			glm::vec4 lightIntensity;
		};

		struct LightBlock
		{
			glm::vec4 ambientIntensity;
			float lightAttenuation;
			float maxIntensity;
			float gamma;
			float padding;
			PerLight lights[NUMBER_OF_LIGHTS];
		} lightBlock;

		glm::mat4 worldToCameraMat;

		unsigned int lightBuffer;
	}

	Arctic::~Arctic()
	{
		glDeleteBuffers(1, &lightBuffer);
	}

	// Time is normalized time of day
	void Arctic::updateLighting(float time)
	{
		lightBlock.lights[0].cameraSpaceLightPos = worldToCameraMat * (glm::angleAxis(time * 2.0f * Constants::PI, vecZ) * sunDirWorld);
		CLEAR_COLOR = skyGradient.getLinear(time);
		lightBlock.lights[0].lightIntensity = sunGradient.getLinear(time);
		lightBlock.maxIntensity = maxGradient.getLinear(time);
		lightBlock.ambientIntensity = ambientGradient.getLinear(time);
	}

	void Arctic::updateLightBuffer()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, lightBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightBlock), &lightBlock);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void Arctic::transformPointLights(const glm::mat4& worldToCamera)
	{
		worldToCameraMat = worldToCamera;
		// start i at 1 since we're assuming lights[0] is directional light
		for (int i{ 1 }; i < NUMBER_OF_LIGHTS; ++i)
		{
			lightBlock.lights[i].cameraSpaceLightPos = worldToCamera * pointPos[i - 1];
		}
	}

	void Arctic::initLights(unsigned int program)
	{
		lightBlock.ambientIntensity = ambientIntensity;
		lightBlock.lightAttenuation = 4.0f * Constants::PI;
		lightBlock.gamma = 1.0f / 2.2f;
		lightBlock.lights[0] = { sunDirWorld, glm::vec4{1.0f} };
		lightBlock.lights[1] = { pointPos[0], pointIntensity[0] };

		// daylight cycle
		skyGradient.insert(4.0f / 24.0f, glm::vec4{ 0.0f, 0.00f, 0.02f, 1.0f });
		skyGradient.insert(6.5f / 24.0f, glm::vec4{ 0.4f, 0.25f, 0.2f, 1.0f });
		skyGradient.insert(11.0f / 24.0f, glm::vec4{ 0.0f, 0.5f, 0.8f, 1.0f });
		skyGradient.insert(14.0f / 24.0f, glm::vec4{ 0.0f, 0.5f, 0.8f, 1.0f });
		skyGradient.insert(18.5f / 24.0f, glm::vec4{ 0.4f, 0.25f, 0.2f, 1.0f });
		skyGradient.insert(21.0f / 24.0f, glm::vec4{ 0.0f, 0.00f, 0.02f, 1.0f });

		sunGradient.insert(4.0f / 24.0f, glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f } *sunBaseIntensity);
		sunGradient.insert(6.5f / 24.0f, glm::vec4{ 0.7f, 0.3f, 0.3f, 1.0f } *sunBaseIntensity);
		sunGradient.insert(8.0f / 24.0f, glm::vec4{ 1.2f, 1.2f, 1.0f, 1.0f } *sunBaseIntensity);
		sunGradient.insert(15.0f / 24.0f, glm::vec4{ 1.1f, 1.0f, 0.9f, 1.0f } *sunBaseIntensity);
		sunGradient.insert(18.5f / 24.0f, glm::vec4{ 0.5f, 0.2f, 0.2f, 1.0f } *sunBaseIntensity);
		sunGradient.insert(21.0f / 24.0f, glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f } *sunBaseIntensity);

		maxGradient.insert(4.0f / 24.0f, nightMaxIntensity);
		maxGradient.insert(8.0f / 24.0f, dayMaxIntensity);
		maxGradient.insert(15.0f / 24.0f, dayMaxIntensity);
		maxGradient.insert(21.0f / 24.0f, nightMaxIntensity);

		ambientGradient.insert(4.0f / 24.0f, nightAmbient);
		ambientGradient.insert(8.0f / 24.0f, dayAmbient);
		ambientGradient.insert(15.0f / 24.0f, dayAmbient);
		ambientGradient.insert(21.0f / 24.0f, nightAmbient);

		glGenBuffers(1, &lightBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, lightBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(lightBlock), &lightBlock, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferBase(GL_UNIFORM_BUFFER, UboBindings::LIGHT, lightBuffer);
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
		igloo = std::make_unique<Object>(&models, "data/mesh/igloo.obj", "data/textures/igloo_col.png", "data/textures/igloo_spec.png");
		pole = std::make_unique<Object>(&models, "data/mesh/pole.obj", "data/textures/pole_col.png");
		bulldozer = std::make_unique<Object>(&models, "data/mesh/bulldozer.obj", "data/textures/bulldozer_col.png");

		activeCamera->transform.pos = glm::vec3{ 0.0f, 1.0f, 5.0f };
		arctic->get().transform.pos.z = -10.0f;
		igloo->get().transform.pos = glm::vec3{ 4.0f, 0.0f, -1.0f };
		pole->get().transform.pos = glm::vec3{ 1.0f, 0.0f, -2.0f };
		bulldozer->get().transform.pos = glm::vec3{ -8.0f, 0.0f, -4.0f };

		bulldozer->get().transform.rot = glm::angleAxis(-Constants::PI / 16.0f, vecY);
		igloo->get().transform.rot = glm::angleAxis(-Constants::PI / 3.0f, vecY);
	}

	// Input vec must be normalized!
	glm::fquat Arctic::quatFromDirection(const glm::vec3& direction)
	{
		float theta{ glm::orientedAngle(glm::vec2{direction.x, direction.z}, glm::vec2{0.0f, 1.0f}) };

		return glm::angleAxis(theta, vecY);
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
			direction = glm::angleAxis(cameraRotation, vecY) * direction;

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
		glm::vec3 cameraRelativePosRotated = glm::angleAxis(cameraRotation, vecY) * cameraRelativePos;
		activeCamera->transform.pos = cameraTarget + cameraRelativePosRotated;

		activeCamera->transform.rot = glm::angleAxis(cameraRotation, vecY);
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

		timeOfDay = (timeOfDay + delta / dayCycleTime <= 1.0f) ? timeOfDay + delta / dayCycleTime : 0.0f;
		updateLighting(timeOfDay);

#ifdef DEBUG
		penguin->get().material.block.specularShininess = guiSpecular;
		penguin->get().material.block.specularColor = glm::vec4{ guiSpecColor, 1.0f };
		penguin->get().material.updateBuffer();

		guiDelta = delta;
		nanoVec[nanoVecIndex++ % NANO_VEC_LEN] = renderTimeNano;
#endif
	}

	void Arctic::gui()
	{
#ifdef DEBUG
		ImGui::Begin("Debug");

		ImGui::SliderFloat("Camera Angle", &guiCameraRotation, 0.0f, 2.0f * Constants::PI);
		ImGui::SliderFloat("Specular", &guiSpecular, 0.0f, 1.0f);
		ImGui::ColorEdit3("Spec Color", (float*)& guiSpecColor); // Edit 3 floats representing a color
		ImGui::Text("Time of day: %.2f hr", timeOfDay * 24);

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