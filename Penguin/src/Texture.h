#pragma once

struct Texture
{
	unsigned int id;
	int width;
	int height;
	int bitsPerPixel;
	unsigned char* buffer;

	Texture(const char* path);
	~Texture();
};