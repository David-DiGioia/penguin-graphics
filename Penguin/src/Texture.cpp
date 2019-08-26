#include "Texture.h"

#include "GL/glew.h"
#include "stb_image/stb_image.h"

Texture::Texture(const char* path)
	: m_textureID{ 0 }, m_width{ 0 }, m_height{ 0 }, m_bitsPerPixel{ 0 }, m_buffer{ nullptr }
{
	stbi_set_flip_vertically_on_load(1);
	m_buffer = stbi_load(path, &m_width, &m_height, &m_bitsPerPixel, 4);

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);   // horizontal wrap
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);   // vertical wrap

	// We're storing the information on how to read out texture in the sampler instead
	// of in the texture itself

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);

	glGenSamplers(1, &m_samplerID);
	glSamplerParameteri(m_samplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_samplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glSamplerParameteri(m_samplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	// If the successive mipmap levels aren't exactly the previous level divided by 2, it will render black
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
	glTexImage2D(GL_TEXTURE_2D, 1, GL_SRGB8_ALPHA8, m_width / 2, m_height / 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_buffer)
		stbi_image_free(m_buffer);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureID);
}

void Texture::bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glBindSampler(slot, m_samplerID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}