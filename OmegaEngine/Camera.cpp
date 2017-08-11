#include "Camera.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace omega {

Camera::Camera(vec3p position, vec3p lookAt, vec3p upward) :
	position(position),
	lookAt(lookAt),
	upward(upward),
	enabled(false) {
}

Camera::~Camera() {
}

void Camera::render() {
}

mat4 Camera::getCameraMatrix(mat4p transform)
{
	vec3 pos = vec3(transform * vec4(position, 1));
	mat3 invTr = glm::inverseTranspose(mat3(transform));
	vec3 dir = invTr * glm::normalize(lookAt - position);
	vec3 upw = invTr * upward;
	return glm::lookAt(pos, pos + dir, upw);
}

}
