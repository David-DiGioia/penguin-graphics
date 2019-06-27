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

std::unique_ptr<Program> program;

std::vector<MeshData::Model> models;
std::vector<VertexBuffer> vbos;
std::vector<VertexArray> vaos;

glm::mat4 cameraToClip;
GLint u_modelToWorld;
GLint u_worldToClip;

std::unique_ptr<MeshData::Camera> camera;
std::unique_ptr<Object> monkey;

void init()
{
	camera = std::make_unique<MeshData::Camera>();
	monkey = std::make_unique<Object>(&models, "data/mesh/susanne.obj", "data/textures/test_grid.png");

	VertexBufferLayout layout;
	layout.Push<float>(3); // position
	layout.Push<float>(2); // textureCoords
	layout.Push<float>(3); // normals

	for (int i{ 0 }; i < models.size(); ++i)
	{
		vaos.emplace_back();
		vaos[i].bind();

		vbos.emplace_back(&models[i].mesh.vertices[0], (unsigned int)(models[i].mesh.vertices.size() * sizeof(MeshData::Vertex)));
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

glm::vec3 position{ 0.0f, 0.0f, -2.0f };
float angle{ 0 };
glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

glm::vec3 positionC{ 0.0f, 0.0f, 0.0f };
float angleC{ 0 };

void update()
{
	glm::vec3 axis{ 0.0f, 1.0f, 0.0f };
	axis = glm::normalize(axis);
	glm::fquat orientation{ glm::angleAxis(angle, axis) };

	monkey->get().transform.pos = position;
	monkey->get().transform.rot = orientation;
	monkey->get().transform.scale = scale;

	glm::fquat orientationC{ glm::angleAxis(angleC, axis) };
	camera->transform.pos = positionC;
	camera->transform.rot = orientationC;
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

	glm::mat4 worldToCamera{ createCameraMatrix(camera->transform) };

	program->bind();
	program->setUniformMat4f(u_worldToClip, cameraToClip * worldToCamera);

	for (int i{ 0 }; i < models.size(); ++i)
	{
		glm::mat4 modelToWorld{ createModelMatrix(models[i].transform) };
		program->setUniformMat4f(u_modelToWorld, modelToWorld);

		vaos[i].bind();
		models[i].colorMap.bind(0);

		glDrawArrays(GL_TRIANGLES, 0, models[i].mesh.vertices.size());
	}
}

void renderGui()
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

void windowResize(GLFWwindow* window, int width, int height)
{
	float aspect{ width / (float)height };
	camera->frustum.r = camera->frustum.t * aspect;
	camera->frustum.l = camera->frustum.b * aspect;
	cameraToClip = Util::createProjMatrix(camera->frustum);

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
		update();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		render();
		renderGui();

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