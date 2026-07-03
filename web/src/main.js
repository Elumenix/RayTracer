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
        const schemaUri = new URL('/RayTracer/schemas/ray-tracer-schema.json', window.location.href).toString();

        // Step 1, configure the yaml for monaco using my schema
        configureMonacoYaml(monaco, {
            enableSchemaRequest: true, // For some reason this needs to be true when passing locally and it won't make a web request, despite what documentation says
            completion: true,
            validate: true,
            hover: true,
            format: true,
            schemas: [
                {
                    uri: schemaUri, // correct file location, so the user can request it via monacos documentation popups
                    fileMatch: ['*'],
                    schema: schema
                }
            ]
        });

        // Step 2, register a color provider for the color arrays in the YAML. This should let monaco recognize color fields and place color decorators
        monaco.languages.registerColorProvider("yaml", {
            provideDocumentColors(model) {
                const text = model.getValue();
                const results = [];

                // Regex to match color arrays in the format: color: [r, g, b]
                // Honestly, just trust the process. I barely understand how regex works but this seems to work well
                const regex = /(color|intensity):\s*\[\s*([\d.]+)\s*,\s*([\d.]+)\s*,\s*([\d.]+)\s*\]/g;

                let match;
                while ((match = regex.exec(text))) {
                    const fieldName = match[1];
                    const r = parseFloat(match[2]);
                    const g = parseFloat(match[3]);
                    const b = parseFloat(match[4]);
                    const color = { red: r, green: g, blue: b, alpha: 1 };

                    // We're only using the array part so that the color decorator doesn't insert before color:
                    const arrayStart = match.index + match[0].indexOf("[");
                    const arrayEnd = match.index + match[0].lastIndexOf("]") + 1;
                    const startPos = model.getPositionAt(arrayStart);
                    const endPos = model.getPositionAt(arrayEnd);

                    // color array and string that represents it
                    results.push({
                        color,
                        range: new monaco.Range(
                            startPos.lineNumber,
                            startPos.column,
                            endPos.lineNumber,
                            endPos.column
                        )
                    });
                }

                return results;
            },

            // Step 3: Setup how the color picker will update the text in the editor when a new color is selected
            provideColorPresentations(model, colorInfo) {
                const color = colorInfo.color;
                const range = colorInfo.range;

                const r = parseFloat(color.red.toFixed(3));
                const g = parseFloat(color.green.toFixed(3));
                const b = parseFloat(color.blue.toFixed(3));

                // We're essentially editing the string to replace the color with values from the color picker
                return [
                    {
                        label: `[${r}, ${g}, ${b}]`,
                        textEdit: {
                            range,
                            text: `[${r}, ${g}, ${b}]`
                        }
                    }
                ];
            }
        });

        // Step 4: Set up how the editor looks and behaves
        const editor = monaco.editor.create(document.getElementById('editor'), {
            // Basics
            value: defaultYaml,
            language: 'yaml',
            theme: 'vs-dark',
            placeholder: 'At minimum, you will need to add a camera here.',

            // Dom Behavior
            automaticLayout: true,
            allowOverflow: false,

            // Behavior
            wordWrap: 'on',
            wordBreak: 'normal',
            wrappingIndent: 'same',
            wrappingStrategy: 'simple',
            scrollBeyondLastLine: false,
            smoothScrolling: true,
            stickyScroll: { enabled: true },
            renderValidationDecorations: 'on',
            folding: true,
            unfoldOnClickAfterEndOfLine: true,
            hover: { enabled: true },
            trimWhitespaceOnDelete: true,

            // Color Decorator
            colorDecorators: true,
            colorDecoratorsActivatedOn: 'click',

            // Indentation (Very important for YAML)
            tabSize: 2,
            useTabStops: true,
            stickyTabStops: true,
            insertSpaces: true,
            detectIndentation: false,
            autoIndent: 'full',
            experimentalWhitespaceRendering: 'off',
            guides: {
                indentation: true,
                highlightActiveIndentation: true,
            },           

            // Active Formatting
            autoClosingBrackets: 'never',
            autoClosingComments: 'never',
            autoClosingDelete: 'never',
            autoClosingOvertype: 'always',
            autoClosingQuotes: 'always',
            autoSurround: 'quotes',

            // Cursor, Copy/Paste, Highlighting, Formatting
            cursorSurroundingLines: 3,
            formatOnType: false,
            formatOnPaste: true,
            autoIndentOnPaste: true,
            autoIndentOnPasteWithinString: false,
            copyWithSyntaxHighlighting: false,
            columnSelection: false, // Kinda cool, might consider enabling later
            roundedSelection: false,
            cursorBlinking: 'smooth',
            multiCursorModifier: 'alt',
            multiCursorPaste: 'spread',
            pasteAs: {enabled: false},

            // Visuals
            minimap: { enabled: false },
            glyphMargin: true,
            lineNumbers: 'on',
            renderWhitespace: 'boundary',
            highlightActiveIndentGuide: true,
            padding: { top: 20, bottom: 25 },

            // Suggestions
            suggest: { // Todo: Look more into this one
                filterGraceful: true,
                localityBonus: true,
                preview: true,
                previewMode: 'prefix',
                matchOnWordStartOnly: false,
                sharedSuggestSelections: true,
                showInlineDetails: true,
                showSnippets: true,
                //snippetsPreventQuickSuggestions: false,
                insertMode: 'insert'
            },
            suggestSelection: 'first',
            quickSuggestions: {
                other: true,
                comments: false,
                strings: true
            },
            snippetSuggestions: 'inline', // Todo: Look more into this one
            inlineSuggest: { // Todo: Look more into this one
                enabled: true,
            },
            suggestOnTriggerCharacters: true,
            acceptSuggestionOnEnter: 'smart',

            // Miscellaneous : (mostly things that default to enabled that don't work in Yaml)
            codeLens: false,
            parameterHints: { enabled: false }
        });


        // Render button event. Here because it needs to read from editor
        document.getElementById('render-btn').addEventListener('click', () => {
            const yaml = editor.getValue()
            const maxDepth = 5 // Make an option for this eventually

            // Calling the c++ render function
            Module.ccall('render', null, ['string', 'number'], [yaml, maxDepth])

            // Error handling - just outputting a message currently
            const error = Module.ccall('get_last_error', 'string', [], [])
            if (error !== "") {
                console.log('error:', error)
            }

            // Retrieve information about the rendered image
            const width = Module.ccall('get_width', 'number', [], [])
            const height = Module.ccall('get_height', 'number', [], [])
            const bufferSize = width * height * 4
            const bufferPtr = Module._malloc(bufferSize)
            Module.ccall('copy_to_buffer', null, ['number'], [bufferPtr])

            // Initialize the image data
            const pixels = new Uint8ClampedArray(Module.HEAPU8.buffer, bufferPtr, bufferSize).slice()
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
