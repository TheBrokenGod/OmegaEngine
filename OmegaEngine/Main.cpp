#define GLM_FORCE_RADIANS
#include "Engine.h"
#include "Importer.h"
#include <iostream>
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_inverse.hpp"

using namespace omega;

static void keyboardCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    	Engine::closeWindow();
    }
}

static void mouseMoveCallback(double x, double y)
{
	std::cout << "mouse at [" << x << ";" << y << "]" << std::endl;
}

static void mouseClickCallback(int button, int action)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		std::cout << "mouse left clicked" << std::endl;
	}
}

static void mouseScrollCallback(double xOff, double yOff) {
	std::cout << "Scroll of " << xOff << " " << yOff << std::endl;
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

int main(int argc, char **argv)
{
	//auto screenSize = Engine::getScreenSize();
	Engine::init(1280, 800, "OpenGL 4.3", false);
	auto scene = Importer::loadScene(argv[1]);
	scene->forEach(printNode);
	scene->find<Camera>("Camera")->enabled = true;
	Engine::setScene(scene);
	Engine::setOnKeyboardEvent(keyboardCallback);
	//Engine::setCursorMode(GLFW_CURSOR_DISABLED);
	//Engine::setOnMouseMove(mouseMoveCallback);
	//Engine::setOnMouseClick(mouseClickCallback);
	//Engine::setOnMouseScroll(mouseScrollCallback);
	Engine::mainLoop();
	return 0;
}
