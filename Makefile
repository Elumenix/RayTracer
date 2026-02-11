.PHONY: build test clean

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

# Clean build files
clean:
	rm -rf build