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
	void setUniform1f(GLint uniform, float f);
	void setUniform3fv(GLint uniform, const glm::vec3& vec);
	void setUniform4fv(GLint uniform, const glm::vec4& vec);
	void setUniformMat3f(GLint uniform, const glm::mat3& matrix);
	void setUniformMat4f(GLint uniform, const glm::mat4& matrix);

	inline unsigned int getID() { return m_id; }

private:
	unsigned int m_id;
};