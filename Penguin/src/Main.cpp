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

// Texture coords range from [0, 1]
float vertexBuffer[]{
	// bottom left
	-0.5f, -0.5f, 0.0f,  // position
	0.0f, 0.0f,          // texture coord

	// top left
	-0.5f, 0.5f, 0.0f,   // position
	0.0f, 1.0f,          // texture coord

	// top right
	0.5f, 0.5f, 0.0f,    // position
	1.0f, 1.0f,          // texture coord

	// bottom right
	0.5f, -0.5f, 0.0f,   // position
	1.0f, 0.0f,          // texture coord
};

unsigned int indices[]{
	0, 1, 2,
	2, 3, 0,
};

std::unique_ptr<Program> program;
std::unique_ptr<Texture> texture;
std::unique_ptr<VertexArray> vao;
std::unique_ptr<VertexBuffer> vbo;
std::unique_ptr<IndexBuffer> ib;

Util::FrustumData frustumData;

glm::mat4 projMat;
glm::mat4 modelMat;
GLint u_cameraToClip;
GLint u_modelToCamera;

void init()
{
	vao = std::make_unique<VertexArray>();
	vao->bind();

	vbo = std::make_unique<VertexBuffer>(vertexBuffer, sizeof(vertexBuffer));
	vbo->bind();

	VertexBufferLayout layout;
	layout.Push<float>(3); // position
	layout.Push<float>(2); // textureCoords

	vao->addBuffer(*vbo, layout);

	ib = std::make_unique<IndexBuffer>(indices, sizeof(indices));

	std::vector<GLuint> shaders{
		Util::compileShader(GL_VERTEX_SHADER, "data/shaders/Shader.vert"),
		Util::compileShader(GL_FRAGMENT_SHADER, "data/shaders/Shader.frag")
	};

	program = std::make_unique<Program>(shaders);
	program->bind();

	texture = std::make_unique<Texture>("data/textures/penguin_t.png");
	texture->bind(0);

	GLint u_texture{ program->getUniform("u_Texture") };
	program->setUniform1i(u_texture, 0);

	// matrices
	u_cameraToClip = program->getUniform("u_cameraToClip");
	u_modelToCamera = program->getUniform("u_modelToCamera");
}

float angle;

void update()
{
	glm::mat4 translate{ glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) };

	glm::vec3 axis{ 0.0f, 1.0f, 0.0f };
	glm::fquat orientation{ glm::angleAxis(angle, axis) };
	modelMat = glm::mat4(1.0f);
	modelMat = translate * glm::toMat4(orientation) * modelMat;
}

void render()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	program->bind();
	program->setUniformMat4f(u_cameraToClip, projMat);
	program->setUniformMat4f(u_modelToCamera, modelMat);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void renderGui()
{
	ImGui::Begin("Debug");
	ImGui::Text("Debug info:");

	ImGui::SliderFloat("Angle", &angle, 0.0f, 2.0f * Constants::PI);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void windowResize(GLFWwindow* window, int width, int height)
{
	float aspect{ width / (float)height };
	frustumData.r = frustumData.t * aspect;
	frustumData.l = frustumData.b * aspect;
	projMat = Util::createProjMatrix(frustumData);

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

	std::cout << glGetString(GL_VERSION) << '\n';

	// Enables transparency in textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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