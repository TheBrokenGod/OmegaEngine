#include "RenderList.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"

namespace omega {

// Default camera is in the origin and directed towards Z
RenderList::RenderList() : cameraMatrix(glm::lookAt(vec3(0, 0, 0), vec3(0, 0, -1), vec3(0, 1, 0))) {
}

void RenderList::add(GraphicNode* object, mat4p transform)
{
	if (dynamic_cast<Mesh*>(object) != nullptr)
	{
		addMesh((Mesh*)object, transform);
	}
	else if (dynamic_cast<Light*>(object) != nullptr)
	{
		addLight((Light*)object, transform);
	}
	else if (dynamic_cast<Camera*>(object) != nullptr)
	{
		setCamera((Camera*)object, transform);
	}
}

void RenderList::addMesh(Mesh* mesh, mat4p transform)
{
	if (!mesh->material->texture->transparent)
	{
		// Opaque meshes are rendered normally with GBuffer
		opaqueMeshes.push_back(std::make_pair(mesh, transform));
	}
	else
	{
		// Transparent meshes go through LLs rendering pipeline
		transparentMeshes.push_back(std::make_pair(mesh, transform));
	}
}

void RenderList::addLight(Light* light, mat4p transform)
{
	if (light->enabled)
	{
		lights.push_back(std::make_pair(light, transform));
	}
}

void RenderList::setCamera(Camera* camera, mat4p transform)
{
	if (camera->enabled)
	{
		cameraMatrix = camera->getCameraMatrix(transform);
		//cameraMatrix = transform * glm::lookAt(camera->position, camera->lookAt, camera->upward);
	}
}

}