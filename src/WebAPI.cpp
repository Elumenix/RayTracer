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
static int g_progress = 0;

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
            // Parse yaml
            printf("Yaml parsing started\n");
            auto scene = YamlParser::getInstance().ParseYaml(std::string(yamlString));
            printf("Yaml parsing completed\n");

            // Extracting isn't necessary, but it makes this so much easier to work with
            Scene::Camera camera = std::move(scene.first);
            Scene::World world = std::move(scene.second);

            // Image size variables. The webpage will read and use these
            g_width = camera.hsize;
            g_height = camera.vsize;

            // Render the scene
            printf("Rendering started\n");
            Rendering::Canvas canvas = camera.Render(world, maxDepth, &g_progress);
            printf("Rendering completed\n");

            // Store in a buffer that javascript can access
            g_canvas = new Rendering::Canvas(std::move(canvas));

            // Notify main thread so that the main thread can notify the web worker (which will then fetch the buffer from c++)
            // I don't really know why it goes to main instead of the worker, but it's better to use worker as an intermediate to keep js code clean
            EM_ASM({
                postMessage({type : "done"});
            });
        }
        catch (const std::exception &e)
        {
            printf("Error occurred: %s\n", e.what());
            g_lastError = e.what();

            EM_ASM({ postMessage({type : "error", error : UTF8ToString($0)}); }, g_lastError.c_str());
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
    int get_progress()
    {
        return g_progress;
    }

    EMSCRIPTEN_KEEPALIVE
    void copy_to_buffer(uint8_t *buffer)
    {
        if (g_canvas)
            g_canvas->CanvasToBuffer(buffer);
    }
}
#endif
