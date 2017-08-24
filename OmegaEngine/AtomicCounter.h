#pragma once

#include "GraphicObject.h"
#include "OpenGL.h"

namespace omega {

class AtomicCounter
{
private:
	GLuint bufferId;
public:
	AtomicCounter(GLenum glUsage);
	~AtomicCounter();
	void bindAndReset();
	GLuint getValue();
};

}

