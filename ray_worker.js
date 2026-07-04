import initWasm from "/RayTracer/raytracer.js";

let Module = null;
let activeRenderId = 0;

onmessage = async (e) => {
    const { yaml, maxDepth, renderId } = e.data;
    activeRenderId = renderId;

    // Initialize WASM module once
    if (!Module) {
        Module = await initWasm();
    }

    // Cancel any previous render
    Module.ccall("cancel_render");

    // Calling the c++ render function
    Module.ccall("render", null, ["string", "number"], [yaml, maxDepth]);

    // Begin polling progress
    pollProgress(renderId);
};

function pollProgress(renderId) {
    // If a new render started, stop polling
    if (renderId !== activeRenderId) return;

    const progress = Module.ccall("get_progress", "number", [], []);
    const total = Module.ccall("get_height", "number", [], []);

    console.log(`Render progress: ${progress+1}/${total}`);

    // Send progress update
    postMessage({ type: "progress", progress: progress + 1, total });

    if ((progress + 1) < total) {
        // Continue polling
        setTimeout(() => pollProgress(renderId), 50);
    }
    else {

        /*
        // Error handling - just outputting a message currently
        const error = Module.ccall('get_last_error', 'string', [], [])
        if (error !== "") {
            console.log('error:', error)
        }
        */

        // Retrieve information about the rendered image
        const width = Module.ccall('get_width', 'number', [], []);
        const height = Module.ccall('get_height', 'number', [], []);
        const bufferSize = width * height * 4;
        const bufferPtr = Module._malloc(bufferSize);
        Module.ccall('copy_to_buffer', null, ['number'], [bufferPtr]);
        const pixels = new Uint8ClampedArray(Module.HEAPU8.buffer, bufferPtr, bufferSize).slice();

        postMessage({
            type: "done",
            width,
            height,
            pixels
        });

        Module.ccall("free", null, ["number"], [bufferPtr]);
    }
}
