#include <gtest/gtest.h>
#include "../src/features/Canvas.h"

TEST(CanvasTest, CreateCanvas) {
    Canvas c = Canvas(10, 20);

    EXPECT_EQ(c.width, 10);
    EXPECT_EQ(c.height, 20);

    Color expectedColor = Color(0,0,0);
    
    
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 10; x++) {
            EXPECT_EQ(c.GetPixelAt(x, y), expectedColor);
        }
    }
} 

TEST(CanvasTest, ChangePixel) {
    Canvas c = Canvas(10,20);
    Color red = Color(1,0,0);

    c.WritePixelAt(2, 3, red);
    EXPECT_EQ(c.GetPixelAt(2,3), red);
}

TEST(CanvasTest, StartString) {
    Canvas c = Canvas(5,3);
    std::string ppm = c.CanvasToPPM();
    std::istringstream iss(ppm);
    std::string ret = "";

    for (int i = 0; i < 3; i++) {
        std::string line;
        getline(iss, line);
        ret += line + "\n";
    }


    std::string expected = "P3\n5 3\n255\n";
    EXPECT_EQ(ret, expected);
}

TEST(CanvasTest, BodyString) {
    Canvas c = Canvas(5,3);
    c.WritePixelAt(0,0,Color(1.5,0,0));
    c.WritePixelAt(2,1,Color(0,0.5,0));
    c.WritePixelAt(4,2,Color(-0.5,0,1));

    std::string ppm = c.CanvasToPPM();
    std::istringstream iss(ppm);

    std::string line;
    //Skip start stuff
    for (int i = 0; i < 3; i++) {
        getline(iss, line);
    }

    getline(iss, line);
    EXPECT_EQ(line, "255 0 0 0 0 0 0 0 0 0 0 0 0 0 0");

    getline(iss, line);
    EXPECT_EQ(line, "0 0 0 0 0 0 0 128 0 0 0 0 0 0 0");

    getline(iss, line);
    EXPECT_EQ(line, "0 0 0 0 0 0 0 0 0 0 0 0 0 0 255");
}

TEST(CanvasTest, SegmentedBodyString) {
    Canvas c = Canvas(10,2);
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 10; x++) {
            c.WritePixelAt(x,y,Color(1,0.8,0.6));
        }
    }

    std::string ppm = c.CanvasToPPM();
    std::istringstream iss(ppm);

    std::string line;
    //Skip start stuff
    for (int i = 0; i < 3; i++) {
        getline(iss, line);
    }

    getline(iss, line);
    EXPECT_EQ(line, "255 204 153 255 204 153 255 204 153 255 204 153 255 204 153 255 204");

    getline(iss, line);
    EXPECT_EQ(line, "153 255 204 153 255 204 153 255 204 153 255 204 153");

    getline(iss, line);
    EXPECT_EQ(line, "255 204 153 255 204 153 255 204 153 255 204 153 255 204 153 255 204");

    getline(iss, line);
    EXPECT_EQ(line, "153 255 204 153 255 204 153 255 204 153 255 204 153");
}