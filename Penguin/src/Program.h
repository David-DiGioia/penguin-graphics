#pragma once

#include <vector>

#include "Core.h"

class Program
{
public:
	Program(const std::vector<GLuint>& shaders);
	~Program();
	void bind() const;
	void unbind() const;
	GLint getUniform(const char* name);
	void setUniform1i(unsigned int uniform, int n);

private:
	unsigned int m_id;
};