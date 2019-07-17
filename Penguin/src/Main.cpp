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
#include "UboBindings.h"

std::unique_ptr<Program> program;
std::unique_ptr<Program> programTriangle;
std::unique_ptr<Program> programBillboard;

std::vector<VertexBuffer> vbos;
std::vector<VertexArray> vaos;

glm::mat4 cameraToClip;
GLint u_cameraSpaceLightPos;
GLint u_lightIntensity;
GLint u_ambientLightIntensity;
GLint u_modelToCamera;
GLint u_cameraToClip;
GLint u_normalModelToCameraMatrix;
GLint u_cameraSpherePos;
GLint u_worldToCamera;

GLuint cameraToClipBuffer;

// NOTICE: CHANGING TYPE OF "scene" DETERMINES WHICH SCENE IS ACTIVE
// -----------------------------------------------------------------
Scenes::Arctic scene;
// -----------------------------------------------------------------

// These coordinates are in clip space....
// The z range is [-1, 1], so they're invisible outside
// this range.
float vertexPositions[]{
	0.5f, 0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
};

float billboardData[]{
	-1.0f, 2.0f, -2.0f, 1.0f,
	-3.0f, 2.0f, -2.0f, 0.5f,
	-2.0f, 1.0f,  0.0f, 0.3f,
};

std::unique_ptr<VertexBuffer> vboPtr;
std::unique_ptr<VertexArray> vaoPtr;

void initTemp()
{
	std::vector<GLuint> shadersBillboard{
		Util::compileShader(GL_VERTEX_SHADER, "data/shaders/TEST.vert"),
		Util::compileShader(GL_FRAGMENT_SHADER, "data/shaders/TEST.frag")
	};

	programTriangle = std::make_unique<Program>(shadersBillboard);

	vboPtr = std::make_unique<VertexBuffer>(vertexPositions, sizeof(vertexPositions));

	vaoPtr = std::make_unique<VertexArray>();
	vaoPtr->bind();

	VertexBufferLayout layout;
	layout.push<float>(3);
	vaoPtr->addBuffer(*vboPtr, layout);
}

void renderTemp()
{
	programTriangle->bind();
	vaoPtr->bind();

	glDrawArrays(GL_TRIANGLES, 0, 3);

	programTriangle->unbind();
}

constexpr int NUMBER_OF_SPHERES{ 3 };

void initBillboard()
{
	std::vector<GLuint> shadersBillboard{
		Util::compileShader(GL_VERTEX_SHADER, "data/shaders/Billboard.vert"),
		Util::compileShader(GL_GEOMETRY_SHADER, "data/shaders/Billboard.geom"),
		Util::compileShader(GL_FRAGMENT_SHADER, "data/shaders/Billboard.frag")
	};

	programBillboard = std::make_unique<Program>(shadersBillboard);
	programBillboard->bind();

	u_worldToCamera = programBillboard->getUniform("u_worldToCamera");

	VertexBufferLayout layout{};
	layout.push<float>(3);  // spherePos
	layout.push<float>(1);  // sphereRadius

	vboPtr = std::make_unique<VertexBuffer>(billboardData, sizeof(billboardData));

	vaoPtr = std::make_unique<VertexArray>();
	vaoPtr->bind();
	vaoPtr->addBuffer(*vboPtr, layout);
}

void init()
{
	initBillboard();

	VertexBufferLayout layout;
	layout.push<float>(3); // position
	layout.push<float>(2); // textureCoords
	layout.push<float>(3); // normals

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
		Util::compileShader(GL_VERTEX_SHADER, "data/shaders/PointLight.vert"),
		Util::compileShader(GL_FRAGMENT_SHADER, "data/shaders/HDR.frag")
	};

	program = std::make_unique<Program>(shaders);
	program->bind();

	GLint u_texture{ program->getUniform("u_texture") };
	program->setUniform1i(u_texture, 0);

	scene.initLights(program->getID());

	// uniforms
	u_modelToCamera = program->getUniform("u_modelToCamera");
	u_normalModelToCameraMatrix = program->getUniform("u_normalModelToCameraMatrix");

	glGenBuffers(1, &cameraToClipBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraToClipBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, UboBindings::CAMERA_TO_CLIP, cameraToClipBuffer);
}

glm::mat4 createCameraMatrix(const MeshData::Transform& transform)
{
	glm::mat4 rotate = glm::toMat4(glm::conjugate(transform.rot));
	glm::mat4 translate = glm::translate(glm::mat4{ 1.0f }, -transform.pos);
	return rotate * translate; // opposite order since they're inverses
}

glm::mat4 createModelMatrix(const MeshData::Transform& transform)
{
	glm::mat4 scale = glm::scale(glm::mat4{ 1.0f }, transform.scale);
	glm::mat4 rotate = glm::toMat4(transform.rot);
	glm::mat4 translate = glm::translate(glm::mat4{ 1.0f }, transform.pos);
	return translate * rotate * scale;
}

void renderBillboard(glm::mat4 worldToCamera)
{
	programBillboard->bind();
	vaoPtr->bind();

	programBillboard->setUniformMat4f(u_worldToCamera, worldToCamera);

	glDrawArrays(GL_POINTS, 0, NUMBER_OF_SPHERES);

	programTriangle->unbind();
}

void render()
{
	glClearColor(scene.CLEAR_COLOR.r, scene.CLEAR_COLOR.g, scene.CLEAR_COLOR.b, scene.CLEAR_COLOR.a);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 worldToCamera{ createCameraMatrix(scene.activeCamera->transform) };

	//program->setUniformMat4f(u_cameraToClip, cameraToClip);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraToClipBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &cameraToClip);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	scene.transformPointLights(worldToCamera);
	scene.updateLightBuffer();

	renderBillboard(worldToCamera);

	program->bind();

	for (int i{ 0 }; i < scene.models.size(); ++i)
	{
		glm::mat4 modelToWorld{ createModelMatrix(scene.models[i].transform) };
		glm::mat4 modelToCamera{ worldToCamera * modelToWorld };

		program->setUniformMat4f(u_modelToCamera, modelToCamera);
		program->setUniformMat3f(u_normalModelToCameraMatrix, modelToCamera);

		vaos[i].bind();
		scene.models[i].material.bind();

		glDrawArrays(GL_TRIANGLES, 0, scene.models[i].mesh.vertices.size());
	}
	program->unbind();
}

void windowResize(GLFWwindow* window, int width, int height)
{
	float aspect{ width / (float)height };
	scene.activeCamera->frustum.r = scene.activeCamera->frustum.t * aspect;
	scene.activeCamera->frustum.l = scene.activeCamera->frustum.b * aspect;
	cameraToClip = Util::createProjMatrix(scene.activeCamera->frustum);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	scene.keyEvent(key, scancode, action, mods);
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

	// Gamma correct then render
	glEnable(GL_FRAMEBUFFER_SRGB);

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
	// Now set the resize callback
	glfwSetWindowSizeCallback(window, windowResize);
	// Key event callback
	glfwSetKeyCallback(window, keyCallback);

	// ImGui stuff
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");

	double currentTime{ 0.0 };
	double lastFrameTime{ 0.0 };
	float deltaTime{ 0.0f };

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		currentTime = glfwGetTime();
		deltaTime = (float)(currentTime - lastFrameTime);
		lastFrameTime = currentTime;

		scene.update(deltaTime);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

#ifdef DEBUG
		Util::Timer timer;
#endif
		render();
#ifdef DEBUG
		scene.renderTimeNano = timer.getNanoseconds();
#endif
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