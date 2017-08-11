#include "Texture.h"
#include "Engine.h"
#include <iostream>

namespace omega {

Texture::Texture(FIBITMAP *bitmap, unsigned width, unsigned height) :
	textureId(0),
	transparent(checkIfTransparent(bitmap, width, height)),
	width(width),
	height(height)
{
	// Build OpenGL texture
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	if (Engine::MaxAnisotropy > 0) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Engine::MaxAnisotropy);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)FreeImage_GetBits(bitmap));
	glGenerateMipmap(GL_TEXTURE_2D);
	FreeImage_Unload(bitmap);
}

bool Texture::checkIfTransparent(FIBITMAP *data, unsigned width, unsigned height)
{
	// If at least one transparent pixel
	for (unsigned y = 0; y < height; y++)
	{
		byte *bits = FreeImage_GetScanLine(data, y);
		for (unsigned x = 0; x < width; x++)
		{
			if (bits[FI_RGBA_ALPHA] < 0xFF) {
				return true;
			}
		}
	}
	return false;
}

std::shared_ptr<Texture> Texture::build(stringp path)
{
	// Load image data from file
	auto format = FreeImage_GetFileType(path.c_str());
	if (format == FIF_UNKNOWN) {
		throw std::runtime_error("FreeImage: error with " + path);
	}
	FIBITMAP *temp = FreeImage_Load(format, path.c_str());
	FIBITMAP *bitmap = FreeImage_ConvertTo32Bits(temp);
	unsigned width = FreeImage_GetWidth(bitmap);
	unsigned height = FreeImage_GetHeight(bitmap);
	FreeImage_Unload(temp);

	// Return built object
	return std::shared_ptr<Texture>(new Texture(bitmap, width, height));
}

void Texture::render() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

Texture::~Texture() {
	glDeleteTextures(1, &textureId);
}

}
