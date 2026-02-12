#include "Canvas.h"

Canvas::Canvas(int width, int height) : width(width), height(height) {
    int size = width * height;
    pixels = std::vector<Color>(width * height, Color(0,0,0));
}

Color Canvas::GetPixelAt(int x, int y)
{
    int index = y * width + x;
    return Color(pixels[index]);
}

void Canvas::WritePixelAt(int x, int y, Color color)
{
    // Error check
    if (x < 0 || y < 0 || x >= width || y >= height) {
        std::cout << "Out of bounds pixel written to: (" << x << ", " << y << ")" << std::endl;
        return;
    }

    int index = y * width + x;
    pixels[index] = color;
}

std::string Canvas::FormatWithLineBreaks(const std::string& input) {
    std::string result;
    std::istringstream iss(input);
    std::string number;
    int currentLineLength = 0;
    
    while (iss >> number) {
        int numberLength = number.length();
        
        // Check if adding this number (plus space if needed) exceeds 70
        if (currentLineLength > 0 && currentLineLength + 1 + numberLength > 70) {
            result += '\n';
            currentLineLength = 0;
        }
        
        // Add space before number if not at start of line
        if (currentLineLength > 0) {
            result += ' ';
            currentLineLength++;
        }
        
        result += number;
        currentLineLength += numberLength;
        
    }
   
    
    return result;
}

std::string Canvas::CanvasToPPM()
{
    std::string ppm = "";
    ppm += "P3\n" + std::to_string(width) + ' ' + std::to_string(height) + "\n255\n";

    int length = width * height;
    std::string activeString = "";

    for (int i = 0; i < length; i++) {
        Color currentPixel = pixels[i];
        activeString += std::to_string((int)round(currentPixel.x * 255)) + " " + std::to_string((int)round(currentPixel.y * 255)) + " " + std::to_string((int)round(currentPixel.z * 255)) + " ";
    }

    return ppm + FormatWithLineBreaks(activeString);
}
