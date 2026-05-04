#pragma once
#include <vector>
#include "Color.h"
#include <string>

namespace Rendering
{
    class Canvas
    {
    private:
        std::vector<Color> pixels;

        // Helper Function
        std::string FormatWithLineBreaks(const std::string &input);

    public:
        const int width;
        const int height;

        Canvas(int width, int height);
        Color GetPixelAt(int x, int y);
        void WritePixelAt(int x, int y, const Color &color);
        void WritePixelAt(float x, float y, const Color &color);
        std::string CanvasToPPM();
        void CanvasToPNG();
    };
}