#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

#include "Core.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

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

std::string stringFromFile(const char* filePath)
{
	std::ifstream stream{ filePath };
	if (!stream)
	{
		std::cerr << "Uh oh, input file could not be opened for reading!\n";
		exit(1);
	}

	std::stringstream buffer;
	buffer << stream.rdbuf();
	return buffer.str();
}

GLuint compileShader(GLenum type, const char* srcPath)
{
	std::string srcString{ stringFromFile(srcPath) };
	const char* src{ srcString.c_str() };

	GLuint shader{ glCreateShader(type) };
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	// Error Checking
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength{ 0 };
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		const char* typeString{ (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT" };

		std::cerr << "ERROR: " << typeString << " SHADER FAILED COMPILATION.\n" << &errorLog[0] << '\n';

		glDeleteShader(shader); // Don't leak the shader.
		DEBUG_BREAK;
	}

	return shader;
}

GLuint generateProgram(const char* vertShaderPath, const char* fragShaderPath)
{
	GLuint vertShader{ compileShader(GL_VERTEX_SHADER, vertShaderPath) };
	GLuint fragShader{ compileShader(GL_FRAGMENT_SHADER, fragShaderPath) };

	GLuint program{ glCreateProgram() };
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)& isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);

		// Use the infoLog as you see fit.
		std::cerr << "Linking program failed.\n";
	}

	glDetachShader(program, vertShader);
	glDetachShader(program, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}

GLuint program;
std::unique_ptr<Texture> texture;
std::unique_ptr<VertexArray> vao;
std::unique_ptr<VertexBuffer> vbo;
std::unique_ptr<IndexBuffer> ib;

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

	program = generateProgram("data/shaders/Shader.vert", "data/shaders/Shader.frag");
	glUseProgram(program);

	texture = std::make_unique<Texture>("data/textures/penguin_t.png");
	texture->bind(0);

	GLint uniform{ glGetUniformLocation(program, "u_Texture") };
	if (uniform == -1)
		std::cout << "uniform == -1\n";
	glUniform1i(uniform, 0);
	glUseProgram(0);
}

void render()
{

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(0);
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

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		render();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}