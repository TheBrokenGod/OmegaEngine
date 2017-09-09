#include "Engine.h"
#include "Importer.h"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

using namespace omega;

static GraphicNode *scene;
const static float MoveSpeed = 50;
const static float PositionBoundary = 240;
static float currentPosition = 0;
static bool movingRight = true;

static void moveObjects(int sign)
{
	currentPosition += sign * MoveSpeed * Engine::deltaTime();
	mat4 matrix = glm::translate(mat4(1), vec3(sign * MoveSpeed * Engine::deltaTime(), 0.f, 0.f));
	matrix = matrix * glm::rotate(mat4(1), Engine::deltaTime(), vec3(1, 0, 0));
	scene->find<Mesh>("TechCube")->matrix *= matrix;
	matrix = glm::translate(mat4(1), vec3(-sign * MoveSpeed * Engine::deltaTime(), 0.f, 0.f));
	matrix = matrix * glm::rotate(mat4(1), Engine::deltaTime(), vec3(1, 0, 0));
	scene->find<Mesh>("EnergySphere")->matrix *= matrix;
}

static void animateObjects()
{
	if (movingRight)
	{
		if (currentPosition < PositionBoundary)
		{
			moveObjects(1);
		}
		else
		{
			movingRight = false;
			animateObjects();
		}
	}
	else
	{
		if (currentPosition > -PositionBoundary)
		{
			moveObjects(-1);
		}
		else
		{
			movingRight = true;
			animateObjects();
		}
	}
}

static void printNode(GraphicNode *node, mat4p transform, int depth)
{
	std::cout << std::string(depth, '-') << node->name;
	std::cout << " : " << typeid(*node).name() << std::endl;
	std::cout << glm::to_string(node->matrix) << std::endl;
	if (dynamic_cast<Light*>(node) != nullptr)
	{
		auto light = (Light*)node;
		auto pos = vec3(transform * vec4(light->position, 1.0));
		std::cout << "light position " << glm::to_string(pos) << std::endl;
		auto dir = glm::inverseTranspose(mat3(transform)) * light->direction;
		std::cout << "light direction " << glm::to_string(dir) << std::endl;
	}
	else if (dynamic_cast<Camera*>(node) != nullptr)
	{
		auto camera = (Camera*)node;
		auto pos = vec3(transform * vec4(camera->position, 1.0));
		std::cout << "camera position " << glm::to_string(pos) << std::endl;
		auto invTr = glm::inverseTranspose(mat3(transform));
		auto dir =  invTr * glm::normalize(camera->lookAt - camera->position);
		std::cout << "camera direction " << glm::to_string(dir) << std::endl;
		std::cout << "camera lookAt " << glm::to_string(invTr * camera->lookAt) << std::endl;
		std::cout << "camera upward " << glm::to_string(invTr * camera->upward) << std::endl;
	}
	std::cout << std::endl;
}

static void keyboardCallback(int key, int action)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		Engine::closeWindow();
	}
}

int main(int argc, char **argv)
{
	//auto screenSize = Engine::getScreenSize();
	auto title = "OIT with linked lists (OpenGL 4.3)";
	Engine::init(1152, 648, title, false);
	scene = Importer::loadScene("../Scene/scene.dae");
	scene->forEach(printNode);
	scene->find<Camera>("Camera")->enabled = true;
	Engine::setScene(scene);
	Engine::setMainLoopCallback(animateObjects);
	Engine::setOnKeyboardEvent(keyboardCallback);
	Engine::mainLoop();
	return 0;
}
