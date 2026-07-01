console.log('main.js is running')
// Get monaco packages
import * as monaco from 'monaco-editor'
import editorWorker from 'monaco-editor/esm/vs/editor/editor.worker?worker'
import { configureMonacoYaml } from 'monaco-yaml'
import yamlWorker from 'monaco-yaml/yaml.worker?worker'

// Variables related to the image display / persistent data
let scaleImage = true;
let savedImage = null;
let debugLetterbox = true;
const editorDiv = document.getElementById("editor");
const canvas = document.getElementById("output");




// ############################################################################
// Button / Image events

document.getElementById('download-btn').addEventListener('click', () => {
    // We'll actually use the saved image canvas for downloading because it contains the rendered image
    const link = document.createElement('a')
    link.download = 'scene.png'
    link.href = savedImage.toDataURL('image/png')
    link.click()
})

document.getElementById('toggle-fit-btn').addEventListener('click', () => {
    scaleImage = !scaleImage
    drawOutputCanvas()
})

function drawNativeCentered(ctx, img) {
    const offsetX = (canvas.width - img.width) / 2;
    const offsetY = (canvas.height - img.height) / 2;

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.drawImage(img, offsetX, offsetY);
}

function drawLetterboxedImage(ctx, img) {
    const canvasAspect = canvas.width / canvas.height;
    const imgAspect = img.width / img.height;
    let drawWidth, drawHeight;

    if (imgAspect > canvasAspect) {
        // Image is wider; letterbox top/bottom
        drawWidth = canvas.width;
        drawHeight = canvas.width / imgAspect;
    } else {
        // Image is taller; letterbox left/right
        drawHeight = canvas.height;
        drawWidth = canvas.height * imgAspect;
    }

    // Calculate the offset to center the image
    const offsetX = (canvas.width - drawWidth) / 2;
    const offsetY = (canvas.height - drawHeight) / 2;

    // Edit the webpage canvas to show the letterboxed image
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.drawImage(img, offsetX, offsetY, drawWidth, drawHeight);

    // Debug visualization
    if (debugLetterbox) {
        ctx.fillStyle = "rgba(255,0,0,0.3)"
        ctx.fillRect(0, 0, canvas.width, offsetY)
        ctx.fillRect(0, canvas.height - offsetY, canvas.width, offsetY)
        ctx.fillRect(0, offsetY, offsetX, drawHeight)
        ctx.fillRect(canvas.width - offsetX, offsetY, offsetX, drawHeight)
    }
}

// Draws stored image to the canvas
function drawOutputCanvas() {

    // If we don't have a saved image, don't do anything
    if (!savedImage) return

    const ctx = canvas.getContext('2d')

    // Set up the webpage canvas to hold the image
    // Canvas resolution should match it's CSS size (in device pixels)
    const rect = canvas.getBoundingClientRect()
    const dpr = window.devicePixelRatio || 1
    canvas.width = rect.width * dpr
    canvas.height = rect.height * dpr
    ctx.setTransform(1, 0, 0, 1, 0, 0)
    ctx.scale(dpr, dpr)

    if (scaleImage) {
        drawLetterboxedImage(ctx, savedImage)
    } else {
        drawNativeCentered(ctx, savedImage)
    }
}

// ############################################################################
// On page load, set up the monaco editor and the YAML schema

// The environment needs the correct workers to handle the programming language (Yaml in this case)
self.MonacoEnvironment = {
    getWorker: function (workerId, label) {
        if (label === 'yaml') {
            // Return the YAML worker if it exists
            return new yamlWorker();
        }

        // Else return the default editor worker
        return new editorWorker();

    }
}

async function loadYamlSchema() {
    const res = await fetch('/RayTracer/schemas/ray-tracer-schema.json');
    return await res.json(); // parse the JSON schema
}

async function loadDefaultYaml() {
    const res = await fetch('/RayTracer/defaults/default.yaml');
    return await res.text(); // no parsing needed
}

Promise.all([loadYamlSchema(), loadDefaultYaml()])
    .then(([schema, defaultYaml]) => {
        configureMonacoYaml(monaco, {
            enableSchemaRequest: true, // For some reason this needs to be true when passing locally and it won't make a web request, despite what documentation says
            completion: true,
            validate: true,
            hover: true,
            format: true,
            schemas: [
                {
                    uri: '/RayTracer/schemas/ray-tracer-schema.json', // correct file location, so the user can request it via monacos documentation popups
                    fileMatch: ['*'],
                    schema: schema
                }
            ]
        });

        // Set up how the editor looks and behaves
        const editor = monaco.editor.create(document.getElementById('editor'), {
            value: defaultYaml,
            language: 'yaml',
            theme: 'vs-dark',
            automaticLayout: true,

            autoIndent: 'full',
            formatOnType: true,
            formatOnPaste: true,
            quickSuggestionsDelay: 0,
            scrollBeyondLastLine: false,
            automaticLayout: false,
            suggest: {
                preview: true,
                detailsVisible: true
            },
            hover: {
                enabled: true,
                delay: 100
            },
            quickSuggestions: {
                other: true,
                comments: false,
                strings: true
            }
        });


        // Render button event. Here because it needs to read from editor
        document.getElementById('render-btn').addEventListener('click', () => {
            const yaml = editor.getValue()
            const maxDepth = 5 // Make an option for this eventually

            // Calling the c++ render function
            Module.ccall('render', null, ['string', 'number'], [yaml, maxDepth])

            // Error handling - just outputting a message currently
            const error = Module.ccall('get_last_error', 'string', [], [])
            console.log('error:', error)

            // Retrieve information about the rendered image
            const width = Module.ccall('get_width', 'number', [], [])
            const height = Module.ccall('get_height', 'number', [], [])
            const bufferSize = width * height * 4
            const bufferPtr = Module._malloc(bufferSize)
            Module.ccall('copy_to_buffer', null, ['number'], [bufferPtr])

            // Initialize the image data
            const pixels = new Uint8ClampedArray(Module.HEAPU8.buffer, bufferPtr, bufferSize)
            const imageData = new ImageData(pixels, width, height)

            // Create an off-screen canvas to draw the image data
            savedImage = document.createElement('canvas')
            savedImage.width = width
            savedImage.height = height
            savedImage.getContext('2d').putImageData(imageData, 0, 0)

            // draw the image on the output canvas
            drawOutputCanvas()

            // Free buffer, and allow image to be downloaded
            Module._free(bufferPtr)
            document.getElementById('download-btn').disabled = false;
        })

        // Match canvas CSS size to editor size
        function resizeCanvasCSS() {
            const rect = editorDiv.getBoundingClientRect();
            canvas.style.width = rect.width + "px";
            canvas.style.height = rect.height + "px";
            editor.layout();
        }

        // They need to be adjusted whenever the window is resized
        window.addEventListener('resize', () => {
            // Resize monaco and the canvas
            editor.layout()
            resizeCanvasCSS()
            drawOutputCanvas()
        })

        // Once everything is loaded, we can make sure the canvas and editor are scaled properly
        resizeCanvasCSS()
    });
