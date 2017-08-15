#include "Mesh.h"

namespace omega {

unsigned Mesh::IdFactory = 0;

Mesh::Mesh(stringp name, float *vertices, float *normals, float *texCoords, int numVertices, std::shared_ptr<Material> material) : 
	meshId(IdFactory++),
	material(material),
	numVertices(numVertices)
{
	this->name = name;
	// Allocate and fill buffers
	glGenBuffers(3, buffersIds);
	glBindBuffer(GL_ARRAY_BUFFER, buffersIds[0]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vec3), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buffersIds[1]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vec3), normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buffersIds[2]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vec3), texCoords, GL_STATIC_DRAW);
}

Mesh::~Mesh() {
	glDeleteBuffers(3, buffersIds);
}

void Mesh::render()
{
	// Set material
	material->render();

	// Bind texture
	material->texture->render();

	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, buffersIds[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, buffersIds[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	// UV coordinates
	glBindBuffer(GL_ARRAY_BUFFER, buffersIds[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(2);

	// Draw mesh
	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

}
