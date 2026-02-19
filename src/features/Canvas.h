#pragma once
#include <vector>
#include "Color.h"
#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <algorithm>

class Canvas {
private:
    std::vector<Color> pixels;

    // Helper Function
    std::string FormatWithLineBreaks(const std::string& input);

public:
    int width, height;

    Canvas(int width, int height);
    Color GetPixelAt(int x, int y);
    void WritePixelAt(int x, int y, Color color);
    void WritePixelAt(float x, float y, Color color);
    std::string CanvasToPPM();
};

