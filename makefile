all:
	@echo "The engine requires the following libraries: glfw, glew, assimp, freeimage. Please make sure they are installed."
	@echo "Starting build process.."
	mkdir -p bin
	g++ -O3 -std=c++11 -ILibraries/IncludesGLM OmegaEngine/*.cpp -lglfw -lGLEW -lGL -lassimp -lfreeimage -lm -o bin/omega
	@echo "Build done."
	@echo "You can now start the test scene by running 'cd bin && ./omega'"

