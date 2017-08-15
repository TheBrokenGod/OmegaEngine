#pragma once

#include "GraphicObject.h"
#include "OpenGL.h"

namespace omega {

class AtomicCounter : public GraphicObject
{
private:
	GLuint bufferId;
public:
	AtomicCounter(GLenum glUsage);
	~AtomicCounter();
	void render();
	void bindAndReset();
	GLuint getValue();
};

}

