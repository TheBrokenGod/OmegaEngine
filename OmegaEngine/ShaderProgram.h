#pragma once

#include "OpenGL.h"
#include "GraphicObject.h"
#include <unordered_map>

namespace omega {

class ShaderProgram : public GraphicObject
{
private:
	GLuint shadersIds[2];
	GLuint programId;
	std::unordered_map<string, GLint> uniforms;
private:
	ShaderProgram(stringp vertex, stringp fragment);
	void compileShader(stringp source, int idIndex);
	void linkProgram();
	GLint getUniformLocation(stringp name);
public:
	static ShaderProgram *fromSource(stringp vertex, stringp fragment);
	static ShaderProgram *fromFiles(stringp vertexFile, stringp fragmentFile);
	~ShaderProgram();
	void use();
	void render();
	void setVector(stringp name, vec3p vector);
	void setVector(stringp name, vec4p vector);
	void setMatrix(stringp name, mat3p matrix);
	void setMatrix(stringp name, mat4p matrix);
	void setValue(stringp name, int value);
	void setValue(stringp name, float value);
};

}

