#include "Program.h"

#include <iostream>
#include "glm/gtc/type_ptr.hpp"

#include "Util.h"

Program::Program(const std::vector<GLuint>& shaders)
{
	m_id = glCreateProgram();

	for (GLuint shader : shaders)
		glAttachShader(m_id, shader);

	glLinkProgram(m_id);

	GLint isLinked = 0;
	glGetProgramiv(m_id, GL_LINK_STATUS, (int*)& isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(m_id);

		std::cerr << "Linking program failed.\n" << &infoLog[0] << '\n';
	}

	for (GLuint shader : shaders)
	{
		glDetachShader(m_id, shader);
		glDeleteShader(shader);
	}
}

Program::~Program()
{
	glDeleteProgram(m_id);
}

void Program::bind() const
{
	glUseProgram(m_id);
}

void Program::unbind() const
{
	glUseProgram(0);
}

GLint Program::getUniform(const char* name)
{
	GLint uniform{ glGetUniformLocation(m_id, name) };
	if (uniform == -1)
		std::cerr << "Warning: glGetUniformLocation for '" << name << "' returned -1\n";
	return uniform;
}

void Program::setUniform1i(GLint uniform, int n)
{
	glUniform1i(uniform, n);
}

void Program::setUniform1f(GLint uniform, float f)
{
	glUniform1f(uniform, f);
}

void Program::setUniform3fv(GLint uniform, const glm::vec3& vec)
{
	glUniform3fv(uniform, 1, glm::value_ptr(vec));
}

void Program::setUniform4fv(GLint uniform, const glm::vec4& vec)
{
	glUniform4fv(uniform, 1, glm::value_ptr(vec));
}

void Program::setUniformMat3f(GLint uniform, const glm::mat3& matrix)
{
	glUniformMatrix3fv(uniform, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Program::setUniformMat4f(GLint uniform, const glm::mat4& matrix)
{
	glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(matrix));
}