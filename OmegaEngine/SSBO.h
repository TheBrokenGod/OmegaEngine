#pragma once

#include "GraphicObject.h"
#include "OpenGL.h"

namespace omega {

class SSBO
{
private:
	GLuint ssboId;
	GLuint bindingIndex;
public:
	SSBO(size_t sizeInBytes, GLuint bindingIndex, GLenum glUsage);
	SSBO(size_t sizeInBytes, GLuint bindingIndex, GLenum glUsage, byte *data);
	~SSBO();
	void bind();
};

}
