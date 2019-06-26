#pragma once

#include "VertexBufferLayout.h"
#include "VertexBuffer.h"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();
	void bind() const;
	void unbind() const;
	void addBuffer(const VertexBuffer& vbo, const VertexBufferLayout& layout);

private:
	unsigned int m_id;
};