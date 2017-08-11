#pragma once

#include "GraphicNode.h"

namespace omega {

typedef enum LightType {
	Directional, Point, Spot
} LightType;

class Light : public GraphicNode
{
private:
	Light(LightType type, vec3p ambient, vec3p diffuse, vec3p specular, vec3 position, vec3 direction, vec3p attenuation, float angle);
public:
	LightType type;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	vec3 direction;
	vec3 attenuation;
	float angle;
	bool enabled;
public:
	~Light();
	static Light* directional(vec3p ambient, vec3p diffuse, vec3p specular, vec3p direction);
	static Light* point(vec3p ambient, vec3p diffuse, vec3p specular, vec3p position, vec3p attenuation);
	static Light* spot(vec3p ambient, vec3p diffuse, vec3p specular, vec3p position, vec3p direction, vec3p attenuation, float angle);
	void render();
};

}
