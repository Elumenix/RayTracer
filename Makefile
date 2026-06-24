.PHONY: setup setupDebug setupRelease build test clean

# Create build directory, necessary after clean
setup:
	cmake -S . -B build -DCMAKE_BUILD_TYPE 

setupDebug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

setupRelease:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build and run only the main executable
build:
	cmake --build build --target RayTracer -j$(nproc)
	./build/RayTracer

# Build and run only the tests
ARGS ?= --gtest_color=yes
test:
	cmake --build build --target run_tests -j$(nproc)
	./build/run_tests $(ARGS)

# Build and test together
all: test build

# Clean build files. Should do before pushing
clean:
	rm -rf build