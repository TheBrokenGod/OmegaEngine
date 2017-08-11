#pragma once

#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include <vector>

namespace omega {

class RenderList
{
private:
	void addMesh(Mesh* mesh, mat4p transform);
	void addLight(Light* light, mat4p transform);
	void setCamera(Camera* camera, mat4p transform);
public:
	mat4 cameraMatrix;
	std::vector<std::pair<Mesh*, mat4>> opaqueMeshes;
	std::vector<std::pair<Mesh*, mat4>> transparentMeshes;
	std::vector<std::pair<Light*, mat4>> lights;
	RenderList();
	void add(GraphicNode* object, mat4p transform);
};

}
