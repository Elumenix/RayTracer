.PHONY: setup setupDebug setupRelease build test clean setupVite buildVite
# Note, current pipline to mimic github is:
# make setupVite -> make buildVite -> cd web && npm run dev 
# This is with both cmake and emscripten set up already on the linux environment (I'm using Ubuntu wsl, things may differ on other systems)

# To instead regression test the C++ code: make setupDebug -> make test
# To run whatever is in the main.cpp file: make setupRelease -> make build

# Clean build files. Should do before pushing
clean:
	rm -rf build

# Create build directory, necessary after clean
setup:
	make clean
	cmake -S . -B build 

# If I need more debug symbols in C++
setupDebug:
	make clean
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# I don't really need this anymore after transfering to Vite, but just in case
setupRelease:
	make clean
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# setup for Vite, also copies the necessary files for the web build
# This needs to be rerun in there is a change to cmakelists.txt (Because this represent what will happen in github actions)
setupVite:
	make clean
	cd web && npm run build
	emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build and run only the main executable
build:
	cmake --build build --target RayTracer -j$(nproc)
	./build/RayTracer

# Build for Vite on the web. Needs to be run after setupVite, and whenever changes are made to the C++ code
# npm run dev can be run in the web folder afterwards to start the development server
buildVite:
	emmake cmake --build build 
	cp build/raytracer.js web/public/raytracer.js
	cp build/raytracer.wasm web/public/raytracer.wasm
	@echo "Now run: cd web && npm run dev"

# Build and run only the tests, ARGS lets me limit it to a specific test
ARGS ?= --gtest_color=yes
test:
	cmake --build build --target run_tests -j$(nproc)
	./build/run_tests $(ARGS)

# Build and test together. Likely outdated this far in the project, but I'll keep it here
all: test build
