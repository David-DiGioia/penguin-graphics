#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "Core.h"

class Program
{
public:
	Program(const std::vector<GLuint>& shaders);
	~Program();
	void bind() const;
	void unbind() const;
	GLint getUniform(const char* name);
	void setUniform1i(GLint uniform, int n);
	void setUniformMat4f(GLint uniform, const glm::mat4& matrix);

private:
	unsigned int m_id;
};