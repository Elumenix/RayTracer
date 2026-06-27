#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <string>
#include <cstdint>
#include "World.h"
#include "Canvas.h"
#include "Camera.h"
#include "YamlParser.h"


// canvas and error message will be available to javascript
static Rendering::Canvas *g_canvas = nullptr;
static int g_width = 0;
static int g_height = 0;
static std::string g_lastError = "";

extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    void render(const char *yamlString, int maxDepth)
    {
        // Clean up previous render
        g_lastError = "";
        delete g_canvas;
        g_canvas = nullptr;

        try
        {
            printf("Yaml paring started\n");

            // Parse yaml
            auto scene = YamlParser::getInstance().ParseYaml(std::string(yamlString));
            printf("Yaml parsing completed\n");
            Scene::Camera camera = std::move(scene.first);
            Scene::World world = std::move(scene.second);
            g_width = camera.hsize;
            g_height = camera.vsize;

            // Render the scene
            printf("Rendering started\n");
            Rendering::Canvas canvas = camera.Render(world, maxDepth);
            printf("Rendering completed\n");
            printf("Canvas created\n");

            // Store in a buffer that javascript can access
            g_canvas = new Rendering::Canvas(std::move(canvas));
        }
        catch (const std::exception &e)
        {
            g_lastError = e.what();
        }
    }

    EMSCRIPTEN_KEEPALIVE
    const char *get_last_error()
    {
        return g_lastError.c_str();
    }

    EMSCRIPTEN_KEEPALIVE
    int get_width()
    {
        return g_width;
    }

    EMSCRIPTEN_KEEPALIVE
    int get_height()
    {
        return g_height;
    }

    EMSCRIPTEN_KEEPALIVE
    void copy_to_buffer(uint8_t *buffer)
    {
        if (g_canvas)
            g_canvas->CanvasToBuffer(buffer);
    }
}
#endif
