.PHONY: setup build test clean

# Create build directory, necessary after clean
setup:
	mkdir -p build
	cd build && cmake ..

# Build and run only the main executable
build:
	cmake --build build --target RayTracer
	./build/RayTracer

# Build and run only the tests
test:
	cmake --build build --target run_tests
	./build/run_tests

# Build and test together
all: test build

# Clean build files. Should do before pushing
clean:
	rm -rf build