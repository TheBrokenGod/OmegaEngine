#pragma once

#include "OpenGL.h"
#include "GraphicNode.h"
#include "RenderList.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Material.h"
#include "Importer.h"
#include "SSBO.h"
#include "AtomicCounter.h"
#include "ShaderProgram.h"
#include <functional>
#include <vector>

namespace omega {

class Engine
{
	friend Material;
	friend Texture;
	friend Importer;
	friend ShaderProgram;
private:
	static GLFWwindow *window;
	static int width, height;
	static int framebufferWidth;
	static int framebufferHeight;
	static GLuint vertexArrayId;
	static GLuint canvasBuffer;
	static std::function<void()> mainLoopCallback;
	static std::function<void(int key, int action)> keyboardCallback;
	static std::function<void(double x, double y)> mouseMoveCallback;
	static std::function<void(int button, int action)> mouseClickCallbck;
	static std::function<void(double x, double y)> mouseScrollCallback;
	static vec3 clearColor;
	static RenderList *renderList;
	static GraphicNode *scene;
	static float MaxAnisotropy;
	static ShaderProgram *activeProgram;
	static ShaderProgram *clearProgram;
	static ShaderProgram *renderProgram;
	static ShaderProgram *blendProgram;
	static AtomicCounter *atomicNodeIndex;
	static SSBO *llHeadsBuffer;
	static SSBO *llFragmentsBuffer;
	static double previousTime;
	static float _deltaTime;
private:
	Engine();
	static void render();
	static void renderMeshes(const std::vector<std::pair<Mesh*, mat4>>& meshes, mat4p projectionMatrix);
	static void privResizeCallback(GLFWwindow *window, int newWidth, int newHeight);
	static void privFbResizeCallback(GLFWwindow *window, int newWidthPixels, int newHeightPixels);
	static void privKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void privMouseMoveCallback(GLFWwindow* window, double xPos, double yPos);
	static void privMouseClickCallback(GLFWwindow* window, int button, int action, int mods);
	static void privScrollCallback(GLFWwindow* window, double xOff, double yOff);
	static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);
	static void buildSSBOs();
	static void deleteSSBOs();
	static mat4 getProjectionMatrix();
	static void drawFullViewportSquare();
	static void activateProgram(ShaderProgram *program);
	static void calcTime();
	static void throwRuntimeError(stringp error);
public:
	static std::pair<int,int> getScreenSize();
	static void init(int width, int height, stringp name, bool fullscreen);
	static vec3 colorFromRGB(int red, int green, int blue);
	static void setClearColor(vec3p color);
	static void setCursorMode(int glfwCursorMode);
	static void setMainLoopCallback(std::function<void()> callback);
	static void setOnKeyboardEvent(std::function<void(int glfwKey, int glfwAction)> callback);
	static void setOnMouseMove(std::function<void(double xPos, double yPos)> callback);
	static void setOnMouseClick(std::function<void(int glfwButton, int glfwAction)> callback);
	static void setOnMouseScroll(std::function<void(double xOff, double yOff)> callback);
	static void setScene(GraphicNode *scene);
	static void resize(int width, int height);
	static int getWidth();
	static int getHeight();
	static void mainLoop();
	static float deltaTime();
	static void closeWindow();
	static void dispose();
};

}
