import initWasm from "/RayTracer/raytracer.js";

let Module = null;
let activeRenderId = 0;

onmessage = async (e) => {
    const message = e.data;

    if (message.type === "done") {
        // Retrieve information about the rendered image
        const width = Module.ccall('get_width', 'number', [], []);
        const height = Module.ccall('get_height', 'number', [], []);
        const bufferSize = width * height * 4;
        const bufferPtr = Module._malloc(bufferSize);
        Module.ccall('copy_to_buffer', null, ['number'], [bufferPtr]);
        const pixels = new Uint8ClampedArray(Module.HEAPU8.buffer, bufferPtr, bufferSize).slice();

        // Send info back to the main thread
        postMessage({
            type: "stream",
            width,
            height,
            pixels
        });

        Module.ccall("free", null, ["number"], [bufferPtr]);
        return;
    }

    // If we make it this far, it instead means that we are calling the render in C++
    const { yaml, renderId } = e.data;
    activeRenderId = renderId;

    // Initialize WASM module once
    if (!Module) {
        Module = await initWasm();
    }

    // Calling the c++ render function
    Module.ccall("render", null, ["string", "number"], [yaml]);
};
