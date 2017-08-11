#include "Importer.h"
#include "assimp/postprocess.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

namespace omega {

GraphicNode* Importer::loadScene(stringp path) {
	return Importer(path).import();
}

Importer::Importer(stringp path) :
	path(toUnixPath(path)),
	directory(parseDir(this->path)),
	scene(nullptr) {
}

GraphicNode* Importer::import()
{
	// Set flags and read scene
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE|aiPrimitiveType_POINT);
	scene = importer.ReadFile(
		path,
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_GenNormals |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_OptimizeMeshes |
		aiProcess_ImproveCacheLocality
	);
	if (scene == nullptr) {
		std::cerr << importer.GetErrorString() << std::endl;
		throw new std::runtime_error(importer.GetErrorString());
	}

	// Import graph
	importCameras();
	importLights();
	importMaterials();
	return importNode(scene->mRootNode);
}

void Importer::importCameras() 
{
	for (unsigned i = 0; i < scene->mNumCameras; ++i)
	{
		auto aiCamera = scene->mCameras[i];
		string name = cvt(aiCamera->mName);
		vec3 position = cvt(aiCamera->mPosition);
		vec3 lookat = cvt(aiCamera->mLookAt);
		vec3 upward = cvt(aiCamera->mUp);
		cameras[name] = new Camera(position, lookat, upward);
	}
}

void Importer::importLights()
{
	for (unsigned i = 0; i < scene->mNumLights; ++i)
	{
		// Convert light parameters
		auto aiLight = scene->mLights[i];
		string name = cvt(aiLight->mName);
		vec3 ambient = cvt(aiLight->mColorAmbient);
		vec3 diffuse = cvt(aiLight->mColorDiffuse);
		vec3 specular = cvt(aiLight->mColorSpecular);
		vec3 position = cvt(aiLight->mPosition);
		vec3 direction = cvt(aiLight->mDirection);
		vec3 attenuation(
			aiLight->mAttenuationConstant,
			aiLight->mAttenuationLinear,
			aiLight->mAttenuationConstant
		);
		float angle = aiLight->mAngleOuterCone;

		Light *light;
		switch (aiLight->mType)
		{
		case aiLightSource_DIRECTIONAL:
			light = Light::directional(ambient, diffuse, specular, direction);
			break;
		case aiLightSource_POINT:
			light = Light::point(ambient, diffuse, specular, position, attenuation);
			break;
		case aiLightSource_SPOT:
			light = Light::spot(ambient, diffuse, specular, position, direction, attenuation, angle);
			break;
		default:
			continue;
		}
		lights[name] = light;
	}
}

void Importer::importMaterials()
{
	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		auto aiMaterial = scene->mMaterials[i];
		aiColor3D ambient, diffuse, specular, emissive;
		float shininess;
		aiString texturePath, materialName;

		aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
		aiMaterial->Get(AI_MATKEY_SHININESS, shininess);
		aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texturePath);
		aiMaterial->Get(AI_MATKEY_NAME, materialName);

		// Load textures from the same directory
		string filename = parseFile(toUnixPath(cvt(texturePath)));
		textures[i] = loadTexture(filename);

		auto material = Material::build(textures[i]);
		material->ambient = cvt(ambient);
		material->diffuse = cvt(diffuse);
		material->specular = cvt(specular);
		material->emissive = cvt(specular);
		material->shininess = shininess;
		material->name = cvt(materialName);
		materials[i] = material;
	}
}

std::shared_ptr<Texture> Importer::loadTexture(stringp name)
{
	for(auto texture : textures) {
		if(name == texture.second->name) {
			return texture.second;
		}
	}
	auto texture = Texture::build(this->directory + name);
	texture->name = name;
	return texture;
}

GraphicNode *Importer::importMesh(const aiNode *aiNode)
{
	auto aiMesh = scene->mMeshes[aiNode->mMeshes[0]];
	auto vertices = (float*) aiMesh->mVertices;
	auto normals = (float*) aiMesh->mNormals;
	auto texCoords = (float*) aiMesh->mTextureCoords[0];
	auto material = materials[aiMesh->mMaterialIndex];
	auto texture = textures[aiMesh->mMaterialIndex];
	auto name = cvt(aiMesh->mName);
	return new Mesh(name, vertices, normals, texCoords, aiMesh->mNumVertices, material);
}

GraphicNode *Importer::importNode(const aiNode *aiNode)
{
	string name = cvt(aiNode->mName);
	mat4 matrix = cvt(aiNode->mTransformation);
	GraphicNode *node = nullptr;

	if (aiNode->mNumMeshes > 0) {
		node = importMesh(aiNode);
	}
	else if (lights.find(name) != lights.end()) {
		node = lights[name];
	}
	else if (cameras.find(name) != cameras.end()) {
		node = cameras[name];
	}
	else {
		node = new GraphicNode;
	}
	node->name = name;
	node->matrix = matrix;

	// Parse children
	for (unsigned i = 0; i < aiNode->mNumChildren; i++)
	{
		GraphicNode *child = importNode(aiNode->mChildren[i]);
		node->attach(child);
	}
	return node;
}

string Importer::toUnixPath(stringp path) {
	string unix = path;
	size_t pos;
	while ((pos = unix.find("\\")) != string::npos) {
		unix.replace(pos, 1, "/");
	}
	return unix;
}

string Importer::parseDir(stringp path) {
	size_t pos = path.find_last_of('/');
	if (pos != string::npos) {
		return path.substr(0, pos + 1);
	}
	return "./";
}

string Importer::parseFile(stringp path) {
	size_t pos = path.find_last_of('/');
	if(pos != string::npos) {
		return path.substr(pos + 1, string::npos);
	}
	return path;
}

vec3 Importer::cvt(aiVector3D aiVector) {
	vec3 vector;
	memcpy(&vector, &aiVector, sizeof(vec3));
	return vector;
}

vec3 Importer::cvt(aiColor3D aiColor) {
	vec3 vector;
	memcpy(&vector, &aiColor, sizeof(vec3));
	return vector;
}

mat4 Importer::cvt(aiMatrix4x4 aiMatrix) {
	mat4 matrix;
	memcpy(&matrix, &aiMatrix, sizeof(mat4));
	matrix = glm::transpose(matrix);
	return matrix;
}

string Importer::cvt(aiString aiString) {
	return string(aiString.C_Str());
}

}
