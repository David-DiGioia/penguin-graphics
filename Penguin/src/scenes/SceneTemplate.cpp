// CHANGE THIS TO YOUR SCENE'S HEADER FILE
#include "SceneTemplate.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "../Constants.h"

namespace Scenes {

	void SceneTemplate::init()
	{
		activeCamera = std::make_unique<MeshData::Camera>();
	}

	void SceneTemplate::update(float delta)
	{

	}

	void SceneTemplate::gui()
	{
		ImGui::Begin("Debug");
		ImGui::Text("Template Scene");
		ImGui::End();
	}

	void SceneTemplate::keyEvent(int key, int scancode, int action, int mods)
	{

	}

}