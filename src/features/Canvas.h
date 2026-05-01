#pragma once
#include <vector>
#include "Color.h"
#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <algorithm>

class Canvas
{
private:
    std::vector<Rendering::Color> pixels;

    // Helper Function
    std::string FormatWithLineBreaks(const std::string &input);

public:
    const int width;
    const int height;

    Canvas(int width, int height);
    Rendering::Color GetPixelAt(int x, int y);
    void WritePixelAt(int x, int y, Rendering::Color color);
    void WritePixelAt(float x, float y, Rendering::Color color);
    std::string CanvasToPPM();
    void CanvasToPNG();
};
