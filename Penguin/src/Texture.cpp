#include "Texture.h"

#include "GL/glew.h"
#include "vendor/stb_image/stb_image.h"

Texture::Texture(const char* path)
	: id{ 0 }, width{ 0 }, height{ 0 }, bitsPerPixel{ 0 }, buffer{ nullptr }
{
	stbi_set_flip_vertically_on_load(1);
	buffer = stbi_load(path, &width, &height, &bitsPerPixel, 4);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);   // horizontal wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);   // vertical wrap

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (buffer)
		stbi_image_free(buffer);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}