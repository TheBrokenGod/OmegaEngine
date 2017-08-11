#pragma once

#include "GraphicNode.h"
#include "Material.h"
#include "Texture.h"
#include "OpenGL.h"
#include <memory>

namespace omega {

class Mesh : public GraphicNode
{
private:
	static unsigned IdFactory;
private:
	GLuint buffersIds[3];
	unsigned numVertices;
public:
	const unsigned meshId;
	std::shared_ptr<Material> material;
	Mesh(stringp name, float *vertices, float *normals, float *texCoords, int numVertices, std::shared_ptr<Material> material);
	~Mesh();
	void render();
};

}
