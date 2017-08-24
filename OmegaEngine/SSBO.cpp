#include "SSBO.h"
#include "Source.h"
#include <iostream>

namespace omega {

SSBO::SSBO(size_t sizeInBytes, GLuint bindingIndex, GLenum glUsage) : SSBO(sizeInBytes, bindingIndex, glUsage, nullptr) {
}

SSBO::SSBO(size_t sizeInBytes, GLuint bindingIndex, GLenum glUsage, byte *data) : ssboId(0), bindingIndex(bindingIndex)
{
	glGenBuffers(1, &ssboId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeInBytes, data, glUsage);
}

SSBO::~SSBO() {
	glDeleteBuffers(1, &ssboId);
}

void SSBO::bind() {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssboId);
}

}