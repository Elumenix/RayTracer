// Get monaco packages
import * as monaco from 'monaco-editor'
import editorWorker from 'monaco-editor/esm/vs/editor/editor.worker?worker'
import { configureMonacoYaml } from 'monaco-yaml'
import yamlWorker from 'monaco-yaml/yaml.worker?worker'
let worker; // No point in assigning a worker off the bat, render is forced to make a new one

// Variables related to the image display / persistent data
let scaleImage = true;
let savedImage = null;
let debugLetterbox = false;
let currentRenderId = 0;
const editorDiv = document.getElementById("editor");
const canvas = document.getElementById("output");
const progressContainer = document.getElementById("render-progress");
const progressBar = progressContainer.querySelector(".progress-bar");
const errorModalBody = document.getElementById("modal-body");
const errorModal = document.getElementById("error-Modal");


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

function updateProgressBar(pct) {
    if (pct === 0) {
        // Temporarily kill the transition so the reset is instant
        progressBar.style.transition = "none";
        progressBar.style.width = "0%";
        progressBar.offsetHeight;
        progressBar.style.transition = "";
    } else {
        progressBar.style.width = pct + "%";
    }
    progressContainer.setAttribute("aria-valuenow", pct);
}

function showProgressBar() {
    progressContainer.classList.remove("d-none");
}

function hideProgressBar() {
    progressContainer.classList.add("d-none");
}

function drawNativeCentered(ctx, img, cssWidth, cssHeight) {
    const offsetX = (cssWidth - img.width) / 2;
    const offsetY = (cssHeight - img.height) / 2;
    ctx.clearRect(0, 0, cssWidth, cssHeight);
    ctx.drawImage(img, offsetX, offsetY);
}

function drawLetterboxedImage(ctx, img, cssWidth, cssHeight) {
    const canvasAspect = cssWidth / cssHeight;
    const imgAspect = img.width / img.height;
    let drawWidth, drawHeight;

    if (imgAspect > canvasAspect) {
        // Image is wider; letterbox top/bottom
        drawWidth = cssWidth;
        drawHeight = cssWidth / imgAspect;
    } else {
        // Image is taller; letterbox left/right
        drawHeight = cssHeight;
        drawWidth = cssHeight * imgAspect;
    }

    // Calculate the offset to center the image
    const offsetX = (cssWidth - drawWidth) / 2;
    const offsetY = (cssHeight - drawHeight) / 2;

    // Edit the webpage canvas to show the letterboxed image
    ctx.clearRect(0, 0, cssWidth, cssHeight);
    ctx.drawImage(img, offsetX, offsetY, drawWidth, drawHeight);

    // Debug visualization
    if (debugLetterbox) {
        ctx.fillStyle = "rgba(255,0,0,0.3)"
        ctx.fillRect(0, 0, cssWidth, offsetY)
        ctx.fillRect(0, cssHeight - offsetY, cssWidth, offsetY)
        ctx.fillRect(0, offsetY, offsetX, drawHeight)
        ctx.fillRect(cssWidth - offsetX, offsetY, offsetX, drawHeight)
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
        drawLetterboxedImage(ctx, savedImage, rect.width, rect.height)
    } else {
        drawNativeCentered(ctx, savedImage, rect.width, rect.height)
    }
}

function drawFinalImage(width, height, pixels) {
    savedImage = document.createElement('canvas');
    savedImage.width = width;
    savedImage.height = height;

    const ctx = savedImage.getContext('2d');
    const imageData = new ImageData(pixels, width, height);
    ctx.putImageData(imageData, 0, 0);

    drawOutputCanvas();
}

// This gets the data from the worker and uses it to update the progress bar and draw the canvas
function handleMessage(e) {
    const { type, progress, total, width, height, pixels, error } = e.data;

    if (type === "progress") {
        // Progress bar only appears once progress is made, so it doesn't look strange popping up for a few frames if yaml fails to parse
        showProgressBar();

        // height is returned as total when the progress type is sent
        const pct = (progress / total) * 100;
        updateProgressBar(pct);
        return;
    }

    if (type === "done") {
        worker.postMessage({ type: "done" });
        return;
    }

    if (type === "stream") {
        drawFinalImage(width, height, pixels);
        updateProgressBar(100);
        hideProgressBar();

        // It now makes sense to allow downloading the image
        document.getElementById('download-btn').disabled = false;
        return;
    }

    // Show error popup if incorrect yaml was submitted
    // This error comes from YamlParser.h in the C++, which has some pretty comprehensive error checking and useful error messages
    if (type === "error") {
        errorModalBody.textContent = error;
        const modal = bootstrap.Modal.getOrCreateInstance(errorModal);
        modal.show();
        hideProgressBar();
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
                const keyedRegex = /(color|intensity):\s*\[\s*([\d.]+)\s*,\s*([\d.]+)\s*,\s*([\d.]+)\s*\]/g;
                let match;
                while ((match = keyedRegex.exec(text))) {
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


                // This is essentially the same as above, it's just targeting pattern colors, which would be in an array instead of a key. So: - [r, g, b]
                const seqItemRegex = /-\s*\[\s*([\d.]+)\s*,\s*([\d.]+)\s*,\s*([\d.]+)\s*\]/g;
                while ((match = seqItemRegex.exec(text))) {
                    const r = parseFloat(match[1]);
                    const g = parseFloat(match[2]);
                    const b = parseFloat(match[3]);
                    const color = { red: r, green: g, blue: b, alpha: 1 };

                    const arrayStart = match.index + match[0].indexOf("[");
                    const arrayEnd = match.index + match[0].lastIndexOf("]") + 1;
                    const startPos = model.getPositionAt(arrayStart);
                    const endPos = model.getPositionAt(arrayEnd);

                    results.push({
                        color,
                        range: new monaco.Range(
                            startPos.lineNumber,
                            startPos.column,
                            endPos.lineNumber,
                            endPos.column)
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
            pasteAs: { enabled: false },

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
            currentRenderId++;

            // Disable if not disabled aready because it's ambiguous what would be downloaded if the user clicks it while a render is in progress
            document.getElementById('download-btn').disabled = true;

            // User specifically wants to render now, so make sure the previous one is cancelled if in progress
            // Ideally we would have a way to detect if a render is finished already, but events are queued and ways to check stall / slow down things much more
            if (worker) worker.terminate();
            worker = new Worker("/RayTracer/ray_worker.js", { type: "module" });
            worker.onmessage = handleMessage;

            // Send a request to the worker to start rendering the scene
            // The worker will send back updates that are handled in the worker.onmessage function above
            worker.postMessage({ yaml, renderId: currentRenderId });
            updateProgressBar(0);
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
