#include "Light.h"

namespace omega {

Light::Light(LightType type, vec3p ambient, vec3p diffuse, vec3p specular, vec3 position, vec3 direction, vec3p attenuation, float angle) :
	type(type),
	ambient(ambient),
	diffuse(diffuse),
	specular(specular),
	position(position),
	direction(direction),
	attenuation(attenuation),
	angle(angle),
	enabled(true) {
}

Light::~Light() {
}

Light* Light::directional(vec3p ambient, vec3p diffuse, vec3p specular, vec3p direction) {
	return new Light(Directional, ambient, diffuse, specular, vec3(0.f), direction, vec3(0.f), 0);
}

Light* Light::point(vec3p ambient, vec3p diffuse, vec3p specular, vec3p position, vec3p attenuation) {
	return new Light(Point, ambient, diffuse, specular, position, vec3(0.f), attenuation, 0);
}

Light* Light::spot(vec3p ambient, vec3p diffuse, vec3p specular, vec3p position, vec3p direction, vec3p attenuation, float angle) {
	return new Light(Spot, ambient, diffuse, specular, position, direction, attenuation, angle);
}

void Light::render() {
}

}
