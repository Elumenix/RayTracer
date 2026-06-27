#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include "Canvas.h"
#include "YamlParser.h"

// canvas and error message will be available to javascript
static Canvas *g_canvas = nullptr;
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
            Camera camera = std::move(scene.first);
            World world = std::move(scene.second);

            // Render the scene
            printf("Rendering started\n");
            Canvas canvas = camera.Render(world, maxDepth);
            printf("Rendering completed\n");
            printf("Canvas created\n");
            // Store in a buffer that javascript can access
            g_canvas = new Canvas(std::move(canvas));
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
        return g_canvas ? g_canvas->GetWidth() : 0;
    }

    EMSCRIPTEN_KEEPALIVE
    int get_height()
    {
        return g_canvas ? g_canvas->GetHeight() : 0;
    }

    EMSCRIPTEN_KEEPALIVE
    void copy_to_buffer(uint8_t *buffer)
    {
        if (g_canvas)
            g_canvas->CopyToBuffer(buffer);
    }
}
#endif