#pragma once

#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include <memory>
#include <unordered_map>

namespace omega {

class Importer
{
private:
	string path;
	string directory;
	const struct aiScene *scene;
	Assimp::Importer importer;
	std::unordered_map<string, Camera*> cameras;
	std::unordered_map<string, Light*> lights;
	std::unordered_map<int, std::shared_ptr<Material>> materials;
	std::unordered_map<int, std::shared_ptr<Texture>> textures;
private:
	void importCameras();
	void importLights();
	void importMaterials();
	std::shared_ptr<Texture>loadTexture(stringp path);
	GraphicNode *importMesh(const aiNode* aiNode);
	GraphicNode *importNode(const aiNode* aiNode);
	static string toUnixPath(stringp path);
	static string parseDir(stringp path);
	static string parseFile(stringp path);
	static vec3 cvt(aiVector3D aiVector);
	static vec3 cvt(aiColor3D aiColor);
	static mat4 cvt(aiMatrix4x4 aiMatrix);
	static string cvt(aiString aString);
	Importer(stringp path);
	GraphicNode* import();
public:
	static GraphicNode* loadScene(stringp path);
};

}
