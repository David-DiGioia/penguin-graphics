#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Core.h"

float vertexBuffer[]{
	-0.5f, -0.5f, 0.0f, // bottom left
	-0.5f, 0.5f, 0.0f,   // top left
	0.5f, 0.5f, 0.0f,   // top right
	0.5f, -0.5f, 0.0f,  // bottom right
};

int indices[]{
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

	glDetachShader(program, vertShader);
	glDetachShader(program, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}

GLuint program;

void init()
{
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Callstack works with errors
	glDebugMessageCallback(MessageCallback, 0);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STATIC_DRAW);

	GLuint elementBuffer;
	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	constexpr int attribArrayIndexPosition{ 0 };
	glVertexAttribPointer(attribArrayIndexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(attribArrayIndexPosition);

	program = generateProgram("data\\Shader.vert", "data\\Shader.frag");
}

void render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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