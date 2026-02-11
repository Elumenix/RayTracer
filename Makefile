.PHONY: setup build test clean

# Create build directory, necessary after clean
setup:
	mkdir -p build
	cd build && cmake ..

# Run build
build:
	cmake --build build
	./build/RayTracer > image.ppm

# Run tests
test:
	cmake --build build
	./build/run_tests

# Build and test together
all: build test

# Clean build files. Should do before pushing
clean:
	rm -rf build