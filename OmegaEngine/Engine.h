#pragma once

#include "OpenGL.h"
#include "GraphicNode.h"
#include "RenderList.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Material.h"
#include "SSBO.h"
#include <functional>

namespace omega {

class Engine
{
	friend Material;
	friend Texture;
private:
	static GLFWwindow *window;
	static int width, height;
	static int framebufferWidth;
	static int framebufferHeight;
	static GLuint vertexArrayId;
	static GLuint canvasBuffer;
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
	static ShaderProgram *opaqueProgram;
	static ShaderProgram *lightingProgram;
	static SSBO *gBuffer;
	static SSBO *llHeadBuffer;
	static SSBO *llDataBuffer;
private:
	Engine();
	static void render();
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
public:
	static std::pair<int,int> getScreenSize();
	static void init(int width, int height, stringp name, bool fullscreen);
	static void setClearColor(vec3p color);
	static void setCursorMode(int glfwCursorMode);
	static void setOnKeyboardEvent(std::function<void(int glfwKey, int glfwAction)> callback);
	static void setOnMouseMove(std::function<void(double xPos, double yPos)> callback);
	static void setOnMouseClick(std::function<void(int glfwButton, int glfwAction)> callback);
	static void setOnMouseScroll(std::function<void(double xOff, double yOff)> callback);
	static void setScene(GraphicNode *scene);
	static void resize(int width, int height);
	static int getWidth();
	static int getHeight();
	static void mainLoop();
	static void closeWindow();
	static void dispose();
};

}
