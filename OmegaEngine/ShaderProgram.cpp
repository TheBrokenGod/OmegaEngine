#include "ShaderProgram.h"
#include "OpenGL.h"
#include "glm/gtc/type_ptr.hpp"
#include <stdexcept>
#include <iostream>

namespace omega {

ShaderProgram::ShaderProgram(stringp vertex, stringp fragment) : GraphicObject(),
	programId(0)
{
	compileShader(vertex, 0);
	compileShader(fragment, 1);
	linkProgram();
}

void ShaderProgram::compileShader(stringp source, int idIndex) 
{
	// Compile shader
	auto type = idIndex == 0 ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
	shadersIds[idIndex] = glCreateShader(type);
	auto cSource = source.c_str();
	glShaderSource(shadersIds[idIndex], 1, &cSource, nullptr);
	glCompileShader(shadersIds[idIndex]);

	// Check outcome
	int status;
	glGetShaderiv(shadersIds[idIndex], GL_COMPILE_STATUS, &status);
	if (!status)
	{
		char log[2048];
		glGetShaderInfoLog(shadersIds[idIndex], 2048, &status, log);
		std::cerr << log << std::endl;
		throw std::runtime_error(string(log));
	}
}

void ShaderProgram::linkProgram() {
	// Build program
	programId = glCreateProgram();
	glAttachShader(programId, shadersIds[0]);
	glAttachShader(programId, shadersIds[1]);
	glLinkProgram(programId);

	// Check outcome
	int status;
	glGetProgramiv(programId, GL_LINK_STATUS, &status);
	char log[2048];
	if (!status)
	{
		glGetProgramInfoLog(programId, 2048, &status, log);
		throw std::runtime_error(string(log));
	}

	glValidateProgram(programId);
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &status);
	if (status == false)
	{
		glGetProgramInfoLog(programId, 2048, &status, log);
		throw std::runtime_error(string(log));
	}
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(programId);
	glDeleteShader(shadersIds[0]);
	glDeleteShader(shadersIds[1]);
}

void ShaderProgram::use() {
	glUseProgram(programId);
}

void ShaderProgram::render() {
	use();
}

int ShaderProgram::getUniformLocation(stringp name)
{
	auto it = uniforms.find(name);
	if (it != uniforms.end()) {
		return it->second;
	}
	uniforms[name] = glGetUniformLocation(programId, name.c_str());
	return uniforms[name];
}

ShaderProgram *ShaderProgram::fromSource(stringp vertex, stringp fragment) {
	return new ShaderProgram(vertex, fragment);
}

ShaderProgram *ShaderProgram::fromFiles(stringp vertexFile, stringp fragmentFile) {
	throw std::logic_error("Not yet implemented");
}

void ShaderProgram::setVector(stringp name, vec3p vector) {
	glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(vector));
}

void ShaderProgram::setVector(stringp name, vec4p vector) {
	glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(vector));
}

void ShaderProgram::setMatrix(stringp name, mat3p matrix) {
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::setMatrix(stringp name, mat4p matrix) {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::setValue(stringp name, int value) {
	glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setValue(stringp name, float value) {
	glUniform1f(getUniformLocation(name), value);
}

}
