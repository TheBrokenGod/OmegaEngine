#pragma once

#include "GraphicObject.h"
#include "OpenGL.h"
#include "FreeImage.h"
#include <memory>

namespace omega {

class Texture : public GraphicObject
{
private:
	GLuint textureId;
	Texture(FIBITMAP *bitmap, unsigned width, unsigned height);
	static bool checkIfTransparent(FIBITMAP *bitmap, unsigned width, unsigned height);
public:
	const bool transparent;
	const unsigned width;
	const unsigned height;
	~Texture();
	static std::shared_ptr<Texture> build(stringp path);
	void render();
};

}
