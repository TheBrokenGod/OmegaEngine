#pragma once

#include "GraphicNode.h"

namespace omega {

class Camera : public GraphicNode
{
public:
	vec3 position;
	vec3 lookAt;
	vec3 upward;
	bool enabled;
public:
	Camera(vec3p position, vec3p lookAt, vec3p upward);
	~Camera();
	mat4 getCameraMatrix(mat4p transform);
};

}
