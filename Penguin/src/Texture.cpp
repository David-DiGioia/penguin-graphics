#include "Texture.h"

#include "GL/glew.h"
#include "stb_image/stb_image.h"

Texture::Texture(const char* path)
	: m_textureID{ 0 }, m_width{ 0 }, m_height{ 0 }, m_bitsPerPixel{ 0 }, m_buffer{ nullptr }
{
	stbi_set_flip_vertically_on_load(1);
	m_buffer = stbi_load(path, &m_width, &m_height, &m_bitsPerPixel, 4);

	// Temp for mimpmap testing!
	unsigned char* red_buffer = stbi_load("data/textures/red.png", &m_width, &m_height, &m_bitsPerPixel, 4);
	unsigned char* green_buffer = stbi_load("data/textures/green.png", &m_width, &m_height, &m_bitsPerPixel, 4);
	unsigned char* blue_buffer = stbi_load("data/textures/blue.png", &m_width, &m_height, &m_bitsPerPixel, 4);
	unsigned char* yellow_buffer = stbi_load("data/textures/yellow.png", &m_width, &m_height, &m_bitsPerPixel, 4);

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);   // horizontal wrap
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);   // vertical wrap

	// We're storing the information on how to read out texture in the sampler instead
	// of in the texture itself

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

	glGenSamplers(1, &m_samplerID);
	glSamplerParameteri(m_samplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_samplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glSamplerParameteri(m_samplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	// Place this directly after defining mipmap level 0, then don't manually define any more
	//glGenerateMipmap(GL_TEXTURE_2D);

	// If the successive mipmap levels aren't exactly the previous level divided by 2, it will render black
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
	glTexImage2D(GL_TEXTURE_2D, 1, GL_SRGB8_ALPHA8, m_width / 2, m_height / 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, red_buffer);
	glTexImage2D(GL_TEXTURE_2D, 2, GL_SRGB8_ALPHA8, m_width / 4, m_height / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, green_buffer);
	glTexImage2D(GL_TEXTURE_2D, 3, GL_SRGB8_ALPHA8, m_width / 8, m_height / 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, blue_buffer);
	glTexImage2D(GL_TEXTURE_2D, 4, GL_SRGB8_ALPHA8, m_width / 16, m_height / 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, yellow_buffer);
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