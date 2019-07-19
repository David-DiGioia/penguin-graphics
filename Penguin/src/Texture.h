#pragma once

class Texture
{
public:
	Texture(const char* path);
	~Texture();
	void bind(unsigned int slot) const;
	void unbind() const;

private:
	unsigned int m_textureID;
	unsigned int m_samplerID;
	int m_width;
	int m_height;
	int m_bitsPerPixel;
	unsigned char* m_buffer;
};