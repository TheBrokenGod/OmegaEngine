#pragma once

#include "GraphicObject.h"
#include "Texture.h"
#include <memory>

namespace omega {

class Material : public GraphicObject
{
private:
	Material(vec3p ambient, vec3p diffuse, vec3p specular, vec3p emissive, float shininess, std::shared_ptr<Texture> texture);

public:
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emissive;
	float shininess;
	std::shared_ptr<Texture> texture;
	static std::shared_ptr<Material> build(std::shared_ptr<Texture> texture);
	static std::shared_ptr<Material> build(vec3p ambient, vec3p diffuse, vec3p specular, vec3p emissive, float shininess, std::shared_ptr<Texture> texture);
	~Material();
	void render();
};

}
