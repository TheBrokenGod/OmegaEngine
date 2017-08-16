all:
	@echo "The engine requires the following libraries: glfw, glew, assimp, freeimage. Please make sure they are installed."
	@echo "Starting build process.."
	g++ -O3 -std=c++11 -ILibraries/IncludesGLM OmegaEngine/*.cpp -lglfw -lGLEW -lGL -lassimp -lfreeimage -lm -o omega
	@echo "Build done."
	@echo "You can now start the program by running './omega Scene/scene.dae'"

