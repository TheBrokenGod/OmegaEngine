#include "Material.h"
#include "Engine.h"

namespace omega {

Material::Material(vec3p ambient, vec3p diffuse, vec3p specular, vec3p emissive, float shininess, std::shared_ptr<Texture> texture) :
	ambient(ambient),
	diffuse(diffuse),
	specular(specular),
	emissive(emissive),
	shininess(shininess),
	texture(texture) {
}

std::shared_ptr<Material> Material::build(std::shared_ptr<Texture> texture) {
	return build(vec3(0.f), vec3(0.f), vec3(0.f), vec3(0.f), 0, texture);
}

std::shared_ptr<Material> Material::build(vec3p ambient, vec3p diffuse, vec3p specular, vec3p emissive, float shininess, std::shared_ptr<Texture> texture) {
	return std::shared_ptr<Material>(new Material(ambient, diffuse, specular, emissive, shininess, texture));
}

Material::~Material() {
}

void Material::render() {
	if (texture->transparent == false)
	{
		// TODO
		//Engine::program->setVector("material.ambient", ambient);
		//Engine::program->setVector("material.diffuse", diffuse);
		//Engine::program->setVector("material.specular", specular);
		//Engine::program->setVector("material.emissive", emissive);
		//Engine::program->setValue("material.shininess", shininess);
	}
}

}
