.PHONY: build

build:
	cmake --build build
	./build/RayTracer > image.ppm