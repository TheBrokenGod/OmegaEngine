#include "Engine.h"
#include "FreeImage.h"
#include "ShaderProgram.h"
#include "Source.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include <stdexcept>
#include <iostream>

namespace omega {

GLFWwindow *Engine::window;
int Engine::width;
int Engine::height;
int Engine::framebufferWidth;
int Engine::framebufferHeight;
GLuint Engine::vertexArrayId;
GLuint Engine::canvasBuffer;
std::function<void()> Engine::mainLoopCallback;
std::function<void(int,int)> Engine::keyboardCallback;
std::function<void(double,double)> Engine::mouseMoveCallback;
std::function<void(int,int)> Engine::mouseClickCallbck;
std::function<void(double,double)> Engine::mouseScrollCallback;
vec3 Engine::clearColor;
RenderList *Engine::renderList;
GraphicNode *Engine::scene;
float Engine::MaxAnisotropy;
ShaderProgram *Engine::activeProgram;
ShaderProgram *Engine::clearProgram;
ShaderProgram *Engine::renderProgram;
ShaderProgram *Engine::blendProgram;
AtomicCounter *Engine::atomicNodeIndex;
SSBO *Engine::llFragmentsBuffer = nullptr;
SSBO *Engine::llHeadsBuffer = nullptr;
double Engine::previousTime;
float Engine::_deltaTime;

Engine::Engine() {
}

std::pair<int,int> Engine::getScreenSize()
{
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialise GLFW");
	}
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    return std::make_pair(mode->width, mode->height);
}

void Engine::init(int width, int height, stringp name, bool fullscreen)
{
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif
	if(!glfwInit()) {
		throw std::runtime_error("Failed to initialise GLFW");
	}
	// Set context params
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	// Create window
	auto monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
	window = glfwCreateWindow(width, height, name.c_str(), monitor, nullptr);
	if(window == nullptr) {
		throw std::runtime_error("Failed to create GLFW window");
	}
	Engine::width = width;
	Engine::height = height;

	// Register callbacks
	glfwSetWindowSizeCallback(window, Engine::privResizeCallback);
	glfwSetFramebufferSizeCallback(window, Engine::privFbResizeCallback);
	glfwSetKeyCallback(window, Engine::privKeyboardCallback);
	glfwSetCursorPosCallback(window, Engine::privMouseMoveCallback);
	glfwSetMouseButtonCallback(window, Engine::privMouseClickCallback);
	glfwSetScrollCallback(window, Engine::privScrollCallback);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	// Setup OpenGL
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Failed to initialize GLEW");
	}
	if (!GLEW_VERSION_4_3) {
		throw std::runtime_error("A 4.3 context is required");
	}
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0, 1);
	glClearDepth(1);
	setClearColor(vec3(0, 0, 0));
	if (GLEW_EXT_texture_filter_anisotropic) {
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Engine::MaxAnisotropy);
	}
	else {
		MaxAnisotropy = 0;
	}
	glEnable(GL_TEXTURE_2D);
#ifdef GLDEBUG
	glDebugMessageCallback((GLDEBUGPROC)Engine::debugCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	// Full viewport square
	vec2 square[] {
		vec2(-1, -1), 
		vec2(+1, -1), 
		vec2(-1, +1), 
		vec2(+1, +1)
	};
	glGenBuffers(1, &canvasBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, canvasBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vec2), square, GL_STATIC_DRAW);

	// Compile shader programs
	activeProgram = nullptr;
	clearProgram = ShaderProgram::fromSource(Source::FullViewportVert, Source::ClearBufferFrag);
	renderProgram = ShaderProgram::fromSource(Source::RenderToListsVert, Source::RenderToListsFrag);
	blendProgram = ShaderProgram::fromSource(Source::FullViewportVert, Source::SortAndBlendFrag);
	
	// SSBOs size depends on the FB size (which is in pixels)
	glfwGetFramebufferSize(window, &Engine::framebufferWidth, &Engine::framebufferHeight);
	atomicNodeIndex = new AtomicCounter(GL_DYNAMIC_COPY);
	buildSSBOs();
	scene = nullptr;
	previousTime = 0;
}

void Engine::buildSSBOs()
{
	deleteSSBOs();
	// The LinkedListsHeadsBuffer holds an atomic reference to the first node of a particular pixel linked list
	llHeadsBuffer = new SSBO(framebufferWidth * framebufferHeight * Source::SizeofLinkedListHeadStd430, 0, GL_DYNAMIC_COPY);
	// The LinkedListsFragmentsBuffer is used to store lists of fragments color for later sorting and blending
	llFragmentsBuffer = new SSBO(Source::FragmentsBufferSizeFactor * framebufferWidth * framebufferHeight * Source::SizeofFragmentNodeStd140, 1, GL_DYNAMIC_COPY);
}

void Engine::deleteSSBOs() {
	delete llFragmentsBuffer;
	llFragmentsBuffer = nullptr;
	delete llHeadsBuffer;
	llHeadsBuffer = nullptr;
}

vec3 Engine::colorFromRGB(int red, int green, int blue) {
	return vec3(red / 255.f, green / 255.f, blue / 255.f);
}

void Engine::setClearColor(vec3p color) {
	clearColor = color;
	glClearColor(color.r, color.g, color.b, 1.f);
}

void Engine::privKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(keyboardCallback && key != GLFW_KEY_UNKNOWN) {
		keyboardCallback(key, action);
	}
}

void Engine::privMouseMoveCallback(GLFWwindow* window, double x, double y)
{
	if(mouseMoveCallback) {
		mouseMoveCallback(x, y);
	}
}

void Engine::privMouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	if(mouseClickCallbck) {
		mouseClickCallbck(button, action);
	}
}

void Engine::privScrollCallback(GLFWwindow* window, double x, double y)
{
	if(mouseScrollCallback) {
		mouseScrollCallback(x, y);
	}
}

void Engine::debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam) {
	std::cout << message << std::endl;
}

void Engine::setCursorMode(int glfwCursorMode) {
	glfwSetInputMode(window, GLFW_CURSOR, glfwCursorMode);
}

void Engine::setMainLoopCallback(std::function<void()> callback) {
	mainLoopCallback = callback;
}

void Engine::setOnKeyboardEvent(std::function<void(int,int)> callback) {
	keyboardCallback = callback;
}

void Engine::setOnMouseMove(std::function<void(double,double)> callback) {
	mouseMoveCallback = callback;
}

void Engine::setOnMouseClick(std::function<void(int,int)> callback) {
	mouseClickCallbck = callback;
}

void Engine::setOnMouseScroll(std::function<void(double,double)> callback) {
	mouseScrollCallback = callback;
}

void Engine::setScene(GraphicNode *scene) {
	Engine::scene = scene;
}

void Engine::mainLoop()
{
	glfwSetTime(0);
	do {
		render();
		calcTime();
	    glfwPollEvents();
		if (mainLoopCallback) {
			mainLoopCallback();
		}
	}
	while(!glfwWindowShouldClose(window));
	dispose();
}

void Engine::render()
{
	// Clear relevant buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	atomicNodeIndex->bindAndReset();
	activateProgram(clearProgram);
	llHeadsBuffer->bind();
	activeProgram->setValue("width", framebufferWidth);
	drawFullViewportSquare();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	if(scene != nullptr)
	{
		renderList = new RenderList();

		// Iterate through the scene graph
		scene->forEach([](GraphicNode* currentNode, mat4p worldMatrix, int)
		{
			renderList->add(currentNode, worldMatrix);
		});
		auto projectionMatrix = getProjectionMatrix();

		// TODO perform depth prepass
		glEnable(GL_DEPTH_TEST);

		// Render transparent meshes
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		activateProgram(renderProgram);
		llHeadsBuffer->bind();
		llFragmentsBuffer->bind();
		renderMeshes(renderList->transparentMeshes, projectionMatrix);

		// Render opaque meshes
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		renderMeshes(renderList->opaqueMeshes, projectionMatrix);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// Sort and blend
		glDisable(GL_DEPTH_TEST);
		activateProgram(blendProgram);
		llHeadsBuffer->bind();
		llFragmentsBuffer->bind();
		activeProgram->setValue("width", framebufferWidth);
		activeProgram->setVector("clearColor", clearColor);
		drawFullViewportSquare();
	    delete renderList;
	}

    glfwSwapBuffers(window);
}

void Engine::renderMeshes(const std::vector<std::pair<Mesh*, mat4>>& meshes, mat4p projectionMatrix)
{
	for (auto mesh : meshes)
	{
		activeProgram->setValue("meshId", (int)mesh.first->meshId);
		mat4 modelvMatrix = renderList->cameraMatrix * mesh.second;
		mat3 normalMatrix = glm::inverseTranspose(mat3(modelvMatrix));
		mat4 renderMatrix = projectionMatrix * modelvMatrix;
		activeProgram->setMatrix("renderMatrix", renderMatrix);
		activeProgram->setMatrix("modelvMatrix", modelvMatrix);
		activeProgram->setMatrix("normalMatrix", normalMatrix);
		activeProgram->setValue("width", framebufferWidth);
		mesh.first->render();
	}
}

void Engine::activateProgram(ShaderProgram *program) {
	activeProgram = program;
	program->use();
}

void Engine::drawFullViewportSquare()
{
	glDisable(GL_DEPTH_TEST);
	glBindBuffer(GL_ARRAY_BUFFER, canvasBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Engine::calcTime() {
	double currentTime = glfwGetTime();
	_deltaTime = float(currentTime - previousTime);
	previousTime = currentTime;
}

float Engine::deltaTime() {
	return _deltaTime;
}

mat4 Engine::getProjectionMatrix() {
	return glm::perspective(glm::radians(45.f), (float)framebufferWidth / framebufferHeight, 0.1f, 1000.0f);
}

void Engine::resize(int width, int height) {
	glfwSetWindowSize(window, width, height);
}

int Engine::getWidth() {
	return width;
}

int Engine::getHeight() {
	return height;
}

void Engine::privResizeCallback(GLFWwindow *window, int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
}

void Engine::privFbResizeCallback(GLFWwindow *window, int newWidthPixels, int newHeightPixels) {
	framebufferWidth = newWidthPixels;
	framebufferHeight = newHeightPixels;
	glViewport(0, 0, newWidthPixels, newHeightPixels);
	buildSSBOs();
}

void Engine::closeWindow() {
	glfwSetWindowShouldClose(window, true);
}

void Engine::dispose() {
	delete scene;
	deleteSSBOs();
	delete atomicNodeIndex;
	delete blendProgram;
	delete renderProgram;
	delete clearProgram;
	glDeleteBuffers(1, &canvasBuffer);
	glDeleteVertexArrays(1, &vertexArrayId);
	glfwDestroyWindow(window);
	glfwTerminate();
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif
}

}