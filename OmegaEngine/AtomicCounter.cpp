#include "AtomicCounter.h"

namespace omega {

AtomicCounter::AtomicCounter(GLenum glUsage)
{
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, bufferId);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, glUsage);
	bindAndReset();
}

AtomicCounter::~AtomicCounter() {
	glDeleteBuffers(1, &bufferId);
}

void AtomicCounter::bindAndReset() {
	GLuint zero = 0;
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, bufferId);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, bufferId);
}

GLuint AtomicCounter::getValue() {
	GLuint value;
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, bufferId);
	glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &value);
	return value;
}

}