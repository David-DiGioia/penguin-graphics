#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <iostream>
#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp""
#include "glm/gtx/quaternion.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Core.h"
#include "Util.h"
#include "Constants.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Program.h"
#include "MeshData.h"
#include "scenes/Scenes.h"

std::unique_ptr<Program> program;

std::vector<VertexBuffer> vbos;
std::vector<VertexArray> vaos;

glm::mat4 cameraToClip;
GLint u_modelToWorld;
GLint u_worldToClip;

// NOTICE: CHANGING TYPE OF "scene" DETERMINES WHICH SCENE IS ACTIVE
// -----------------------------------------------------------------
Scenes::SceneTest scene;
// -----------------------------------------------------------------

void init()
{
	VertexBufferLayout layout;
	layout.Push<float>(3); // position
	layout.Push<float>(2); // textureCoords
	layout.Push<float>(3); // normals


	// Beware! If vector has to reallocate for vaos or vbos, destructors will be called, deleting gl buffers
	for (int i{ 0 }; i < scene.models.size(); ++i)
	{
		vaos.emplace_back();
		vaos[i].bind();

		vbos.emplace_back(&scene.models[i].mesh.vertices[0],
			(unsigned int)(scene.models[i].mesh.vertices.size() * sizeof(MeshData::Vertex)));
		vbos[i].bind();

		vaos[i].addBuffer(vbos[i], layout);
	}

	std::vector<GLuint> shaders{
		Util::compileShader(GL_VERTEX_SHADER, "data/shaders/Shader.vert"),
		Util::compileShader(GL_FRAGMENT_SHADER, "data/shaders/Shader.frag")
	};

	program = std::make_unique<Program>(shaders);
	program->bind();

	GLint u_texture{ program->getUniform("u_Texture") };
	program->setUniform1i(u_texture, 0);

	// matrices
	u_modelToWorld = program->getUniform("u_modelToWorld");
	u_worldToClip = program->getUniform("u_worldToClip");
}

glm::mat4 createCameraMatrix(const MeshData::Transform& transform)
{
	glm::mat4 rotate = glm::toMat4(glm::conjugate(transform.rot));
	glm::mat4 translate = glm::translate(glm::mat4{ 1.0f }, -transform.pos);
	return translate * rotate;
}

glm::mat4 createModelMatrix(const MeshData::Transform& transform)
{
	glm::mat4 scale = glm::scale(glm::mat4{ 1.0f }, transform.scale);
	glm::mat4 rotate = glm::toMat4(transform.rot);
	glm::mat4 translate = glm::translate(glm::mat4{ 1.0f }, transform.pos);
	return translate * rotate * scale;
}

void render()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 worldToCamera{ createCameraMatrix(scene.activeCamera->transform) };

	program->bind();
	program->setUniformMat4f(u_worldToClip, cameraToClip * worldToCamera);

	for (int i{ 0 }; i < scene.models.size(); ++i)
	{
		glm::mat4 modelToWorld{ createModelMatrix(scene.models[i].transform) };
		program->setUniformMat4f(u_modelToWorld, modelToWorld);

		vaos[i].bind();
		scene.models[i].colorMap.bind(0);

		glDrawArrays(GL_TRIANGLES, 0, scene.models[i].mesh.vertices.size());
	}
}

void windowResize(GLFWwindow* window, int width, int height)
{
	float aspect{ width / (float)height };
	scene.activeCamera->frustum.r = scene.activeCamera->frustum.t * aspect;
	scene.activeCamera->frustum.l = scene.activeCamera->frustum.b * aspect;
	cameraToClip = Util::createProjMatrix(scene.activeCamera->frustum);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
		std::cout << "Error\n";

	std::cout << glGetString(GL_VERSION) << "\n\n";

	// Enables transparency in textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	// Enable debug stuff
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Callstack works with errors
	glDebugMessageCallback(MessageCallback, 0);

	// Warning!! Update MAX_MODELS if you need more than 16 models or vectors will have to reallocate
	// and that deletes gl objects that we still need
	scene.models.reserve(scene.MAX_MODELS);
	vbos.reserve(scene.MAX_MODELS);
	vaos.reserve(scene.MAX_MODELS);

	scene.init();
	init();

	// Initial call to resize so matrix has correct aspect
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		windowResize(window, width, height);
	}
	// Now set the callback
	glfwSetWindowSizeCallback(window, windowResize);

	// ImGui stuff
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		scene.update();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		render();
		scene.gui();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}