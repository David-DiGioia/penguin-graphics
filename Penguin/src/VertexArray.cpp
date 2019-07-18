#include "VertexArray.h"

#include "Core.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_id);
}

void VertexArray::bind() const
{
	glBindVertexArray(m_id);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::addBuffer(const VertexBuffer& vbo, const VertexBufferLayout& layout)
{
	bind();
	vbo.bind();
	const auto& elements{ layout.getElements() };
	unsigned int offset{ 0 };
	for (unsigned int i{ 0 }; i < elements.size(); ++i)
	{
		const VertexBufferElement& element{ elements[i] };
		glEnableVertexAttribArray(i);
		if (element.integerType)
			glVertexAttribIPointer(i, element.count, element.type,
				layout.getStride(), (const void*)offset);
		else
			glVertexAttribPointer(i, element.count, element.type,
				element.normalized, layout.getStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::getSizeOfType(element.type);
	}
}