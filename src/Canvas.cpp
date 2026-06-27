#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Canvas.h"
#include "Color.h"
#include "stb_image_write.h" // CMake includes this, it doesn't exist until "make setup" is called
#include <iostream>
#include <algorithm>
#include <sstream>

namespace Rendering
{
    Canvas::Canvas(int width, int height) : width(width), height(height)
    {
        int size = width * height;
        pixels = std::vector<Color>(width * height, Color(0, 0, 0));
    }

    Color Canvas::GetPixelAt(int x, int y)
    {
        int index = y * width + x;
        return Color(pixels[index]);
    }

    void Canvas::WritePixelAt(int x, int y, const Color &color)
    {
        // Error check
        if (x < 0 || y < 0 || x >= width || y >= height)
        {
            std::cout << "Out of bounds pixel written to: (" << x << ", " << y << ")" << std::endl;
            return;
        }

        int index = y * width + x;
        pixels[index] = color;
    }

    void Canvas::WritePixelAt(float x, float y, const Color &color)
    {
        int xInt = std::round(x);
        int yInt = std::round(y);
        WritePixelAt(xInt, yInt, color);
    }

    std::string Canvas::FormatWithLineBreaks(const std::string &input)
    {
        std::string result;
        std::istringstream iss(input);
        std::string number = "";

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {

                // Amount of numbers before a forced newLine. This will happen regardless if length exceeds 70
                int lineLength = width * 3;
                int currentLineLength = 0;

                for (int i = 0; i < lineLength; i++)
                {
                    iss >> number;
                    int numberLength = number.length();

                    // Check if adding this number (plus space before) exceeds 70
                    if (currentLineLength > 0 && currentLineLength + 1 + numberLength > 70)
                    {
                        result += '\n';
                        currentLineLength = 0;
                    }

                    // Add space before number if not at start of line
                    if (currentLineLength > 0)
                    {
                        result += " ";
                        currentLineLength++;
                    }

                    result += number;
                    currentLineLength += numberLength;
                }

                // NewLine between rows regardless
                result += '\n';
            }
        }

        return result;
    }

    std::string Canvas::CanvasToPPM()
    {
        std::string ppm = "";
        ppm += "P3\n" + std::to_string(width) + ' ' + std::to_string(height) + "\n255\n";

        int length = width * height;
        std::string activeString = "";

        for (int i = 0; i < length; i++)
        {
            Color currentPixel = pixels[i];
            activeString += std::to_string(std::clamp((int)std::round(currentPixel.x * 255), 0, 255)) + " " +
                            std::to_string(std::clamp((int)std::round(currentPixel.y * 255), 0, 255)) + " " +
                            std::to_string(std::clamp((int)std::round(currentPixel.z * 255), 0, 255)) + " ";
        }

        return ppm + FormatWithLineBreaks(activeString);
    }

    void Canvas::CanvasToPNG()
    {
        // Set up the data for a png
        std::vector<unsigned char> raw(width * height * 3);
        for (int i = 0; i < width * height; i++)
        {
            raw[i * 3 + 0] = static_cast<unsigned char>(std::clamp(pixels[i].x, 0.0f, 1.0f) * 255);
            raw[i * 3 + 1] = static_cast<unsigned char>(std::clamp(pixels[i].y, 0.0f, 1.0f) * 255);
            raw[i * 3 + 2] = static_cast<unsigned char>(std::clamp(pixels[i].z, 0.0f, 1.0f) * 255);
        }

        int result = stbi_write_png("Scene.png", width, height, 3, raw.data(), width * 3);

        if (result == 0)
        {
            std::cerr << "Failed to write Scene.png" << std::endl;
        }
    }

    void Canvas::CanvasToBuffer(uint8_t *buffer)
    {
        int n = width * height;

        // 4 channels per pixel because the browser expects RGBA
        for (int i = 0; i < n; i++)
        {
            buffer[i * 4 + 0] = static_cast<uint8_t>(std::clamp(pixels[i].x, 0.0f, 1.0f) * 255);
            buffer[i * 4 + 1] = static_cast<uint8_t>(std::clamp(pixels[i].y, 0.0f, 1.0f) * 255);
            buffer[i * 4 + 2] = static_cast<uint8_t>(std::clamp(pixels[i].z, 0.0f, 1.0f) * 255);
            buffer[i * 4 + 3] = 255; // Fully opaque
        }
    }
}
