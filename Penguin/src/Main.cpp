#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

#include "Core.h"
#include "Texture.h"

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
		// Don't leak shaders either.

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

	program = generateProgram("data/shaders/Shader.vert", "data/shaders/Shader.frag");
	glUseProgram(program);

	texture = std::make_unique<Texture>("data/textures/penguin.png");
	// Use texture slot 0. Later this should not be hardcoded.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	GLint uniform{ glGetUniformLocation(program, "u_Texture") };
	if (uniform == -1)
		std::cout << "uniform == -1\n";
	glUniform1i(uniform, 0);
	glUseProgram(0);

	constexpr int indexPosition{ 0 };
	constexpr int indexTexCoord{ 1 };
	int stride{ 5 * sizeof(float) };
	glVertexAttribPointer(indexPosition, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(indexTexCoord, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(indexPosition);
	glEnableVertexAttribArray(indexTexCoord);
}

// Note: I think texture was failing when declared globally because it calls
// gl functions before the context has been created.
// Q: So why does it fail when created in init function?
// A: Destructor is called.

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