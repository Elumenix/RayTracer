console.log('main.js is running')
// Get monaco packages
import * as monaco from 'monaco-editor'
import editorWorker from 'monaco-editor/esm/vs/editor/editor.worker?worker'
import { configureMonacoYaml } from 'monaco-yaml'
import yamlWorker from 'monaco-yaml/yaml.worker?worker'

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

console.log('Module exists:', typeof window.Module)
console.log('raytracer.js loaded:', typeof Module !== 'undefined')


configureMonacoYaml(monaco, {
    enableSchemaRequest: false, // No reason to request schemas from the internet. The raytracer is custom
    schemas: [
        {
            uri: 'http://raytracer/scene-schema.json', // schema name. Dot used as url becuase enableSchemaRequest is false
            fileMatch: ['*'], // Use the schema for all files
            schema: { // json-schema - draft 07 style is apparently correct here
                type: 'array',
                items: {
                    oneOf: [
                        { $ref: '#/definitions/addCamera' },
                        { $ref: '#/definitions/addLight' },
                        { $ref: '#/definitions/addPlane' },
                        { $ref: '#/definitions/addSphere' },
                        { $ref: '#/definitions/addCube' },
                        { $ref: '#/definitions/defineEntry' }
                    ]
                },
                definitions: {
                    // Helper definitions for vectors
                    vec3: {
                        type: 'array',
                        items: {
                            type: 'number',
                        },
                        minItems: 3,
                        maxItems: 3,
                        default: [0, 0, 0]
                    },
                    vec3Clamped: {
                        type: 'array',
                        items: {
                            type: 'number',
                            minimum: 0,
                            maximum: 1
                        },
                        minItems: 3,
                        maxItems: 3,
                        default: [1, 1, 1]
                    },


                    // Material/Transform definitions (This kind of hurts)
                    //###############################################################################################################################
                    material: {
                        description: 'Defines what the surface of an object looks like.',
                        type: 'object',
                        properties: {
                            color: {
                                description: 'The base color of the surface. Default is white',
                                $ref: '#/definitions/vec3Clamped',
                                default: [1, 1, 1]
                            },
                            diffuse: {
                                description: 'How much the surface scatters light in all directions when hit by a light source. Higher values make the surface appear more matte.',
                                type: 'number',
                                minimum: 0,
                                maximum: 1,
                                default: 0.9
                            },
                            ambient: {
                                description: 'How much the surface is lit even when not directly facing a light source. Prevents completely unlit areas from being pure black.',
                                type: 'number',
                                minimum: 0,
                                maximum: 1,
                                default: 0.1
                            },
                            specular: {
                                description: 'The brightness of the shiny highlight that appears where a light source reflects toward the camera.',
                                type: 'number',
                                minimum: 0,
                                maximum: 1,
                                default: 0.9
                            },
                            shininess: {
                                description: 'How tight/small the specular highlight is. Higher values make the surface look more polished and less rough.',
                                type: 'number',
                                minimum: 0,
                                default: 200
                            },
                            reflective: {
                                description: 'How much the surface reflects light. Higher values make the surface more reflective.',
                                type: 'number',
                                minimum: 0,
                                maximum: 1,
                                default: 0
                            },
                            transparency: {
                                description: 'How much light passes through the surface. 0 is fully opaque, 1 is fully transparent.',
                                type: 'number',
                                minimum: 0,
                                maximum: 1,
                                default: 0
                            },
                            'refractive-index': {
                                description: 'How much light bends when passing through a transparent surface. Some examples: air: 1.0, water: 1.33, glass: 1.5.',
                                type: 'number',
                                minimum: 0,
                                default: 1.0
                            }
                        }
                    },
                    transform: {
                        description: 'Determines the position, rotation, and scale of an object in the scene.\nA transform is essentially a list of transformations to apply to an object.',
                        type: 'array',
                        items: {
                            oneOf: [
                                {
                                    description: 'A reference to a previously defined transform.',
                                    type: 'string'
                                },
                                {
                                    description: 'Move the position of the object in 3D space.',
                                    type: 'array',
                                    items: [
                                        { const: 'translate' },
                                        { type: 'number', description: 'x' },
                                        { type: 'number', description: 'y' },
                                        { type: 'number', description: 'z' }
                                    ],
                                    minItems: 4,
                                    maxItems: 4,
                                    default: ['translate', 0, 0, 0]
                                },
                                {
                                    description: 'Multiply the scale of the object.',
                                    type: 'array',
                                    items: [
                                        { const: 'scale' },
                                        { type: 'number', description: 'x' },
                                        { type: 'number', description: 'y' },
                                        { type: 'number', description: 'z' }
                                    ],
                                    minItems: 4,
                                    maxItems: 4,
                                    default: ['scale', 1, 1, 1]
                                },
                                {
                                    description: 'Rotate the object around the worlds X axis.',
                                    type: 'array',
                                    items: [
                                        { const: 'rotate-x' },
                                        { type: 'number', description: 'radians' },
                                    ],
                                    minItems: 2,
                                    maxItems: 2,
                                    default: ['rotate-x', 0]
                                },
                                {
                                    description: 'Rotate the object around the worlds Y axis.',
                                    type: 'array',
                                    items: [
                                        { const: 'rotate-y' },
                                        { type: 'number', description: 'radians' },
                                    ],
                                    minItems: 2,
                                    maxItems: 2,
                                    default: ['rotate-y', 0]
                                },
                                {
                                    description: 'Rotate the object around the worlds Z axis.',
                                    type: 'array',
                                    items: [
                                        { const: 'rotate-z' },
                                        { type: 'number', description: 'radians' },
                                    ],
                                    minItems: 2,
                                    maxItems: 2,
                                    default: ['rotate-z', 0]
                                },
                                {
                                    description: 'Shear the object along the specified axes.',
                                    type: 'array',
                                    items: [
                                        { const: 'shear' },
                                        { type: 'number', description: 'xy' },
                                        { type: 'number', description: 'xz' },
                                        { type: 'number', description: 'yx' },
                                        { type: 'number', description: 'yz' },
                                        { type: 'number', description: 'zx' },
                                        { type: 'number', description: 'zy' }
                                    ],
                                    minItems: 7,
                                    maxItems: 7,
                                    default: ['shear', 0, 0, 0, 0, 0, 0]
                                }
                            ]
                        }
                    },


                    // Scene Objects
                    //###############################################################################################################################
                    addCamera: {
                        type: 'object',
                        properties: {
                            add: {
                                description: 'The camera settings for the scene. Controls the direction, fov, and image size generated.\nOnly one camera can be added to a scene.',
                                const: 'camera'
                            },
                            width: {
                                description: 'The width (in pixels) of the generated image.',
                                type: 'integer',
                                minimum: 1,
                                default: 128
                            },
                            height: {
                                description: 'The height (in pixels) of the generated image.',
                                type: 'integer',
                                minimum: 1,
                                default: 128
                            },
                            'field-of-view': {
                                description: 'The field of view of the camera.',
                                type: 'number',
                                exclusiveMinimum: 0,
                                exclusiveMaximum: 3.14159,
                                default: 1.0472
                            },
                            'from': {
                                description: 'The position of the camera in 3D space.',
                                $ref: '#/definitions/vec3',
                                default: [0, 0, 0]
                            },
                            'to': {
                                description: 'The point the camera is looking at in 3D space.',
                                $ref: '#/definitions/vec3',
                                default: [0, 0, -1]
                            },
                            'up': {
                                description: 'The up vector of the camera in 3D space.',
                                $ref: '#/definitions/vec3',
                                default: [0, 1, 0]
                            }
                        },
                        required: ['add', 'width', 'height', 'field-of-view']
                    },
                    addLight: {
                        type: 'object',
                        properties: {
                            add: {
                                description: 'A point light that will light up things around it.',
                                const: 'light'
                            },
                            at: {
                                description: 'The position of the light in 3D space.',
                                $ref: '#/definitions/vec3',
                                default: [0, 0, 0]
                            },
                            intensity: {
                                description: 'The color of the light.',
                                $ref: '#/definitions/vec3Clamped',
                                default: [1, 1, 1]
                            },
                        },
                        required: ['add', 'at', 'intensity']
                    },
                    addPlane: {
                        type: 'object',
                        properties: {
                            add: {
                                description: 'A plane that will be added to the scene.',
                                const: 'plane'
                            },
                            material: {
                                description: 'The material of the plane.',
                                oneOf: [
                                    {
                                        $ref: '#/definitions/material',
                                        default: {}
                                    },
                                    {
                                        type: 'string',
                                        default: 'parent'
                                    }
                                ]
                            },
                            transform: {
                                description: 'The transform of the plane.',
                                oneOf: [
                                    {
                                        $ref: '#/definitions/transform',
                                        default: {}
                                    },
                                    {
                                        type: 'string',
                                        default: 'parent'
                                    }
                                ]
                            }
                        },
                        required: ['add', 'material', 'transform']
                    },
                    addSphere: {
                        type: 'object',
                        properties: {
                            add: {
                                description: 'A sphere that will be added to the scene.',
                                const: 'sphere'
                            },
                            material: {
                                description: 'The material of the sphere.',
                                oneOf: [
                                    {
                                        $ref: '#/definitions/material',
                                        default: {}
                                    },
                                    {
                                        type: 'string',
                                        default: 'parent'
                                    }
                                ]
                            },
                            transform: {
                                description: 'The transform of the sphere.',
                                oneOf: [
                                    {
                                        $ref: '#/definitions/transform',
                                        default: {}
                                    },
                                    {
                                        type: 'string',
                                        default: 'parent'
                                    }
                                ]
                            }
                        },
                        required: ['add', 'material', 'transform']
                    },
                    addCube: {
                        type: 'object',
                        properties: {
                            add: {
                                description: 'A cube that will be added to the scene.',
                                const: 'cube'
                            },
                            material: {
                                description: 'The material of the cube.',
                                oneOf: [
                                    {
                                        $ref: '#/definitions/material',
                                        default: {}
                                    },
                                    {
                                        type: 'string',
                                        default: 'parent'
                                    }
                                ]
                            },
                            transform: {
                                description: 'The transform of the cube.',
                                oneOf: [
                                    {
                                        $ref: '#/definitions/transform',
                                        default: {}
                                    },
                                    {
                                        type: 'string',
                                        default: 'parent'
                                    }
                                ]
                            }
                        },
                        required: ['add', 'material', 'transform']
                    },


                    // Define/Extend materials and transforms
                    //###############################################################################################################################
                    defineEntry: {
                        type: 'object',
                        properties: {
                            define: {
                                description: 'The name of the new material or transform.',
                                type: 'string',
                                default: 'unique-name'
                            },
                            extend: {
                                description: 'The name of the material or transform to extend.\nOn a transform, an extend is optional as the name of the extended transform can instead be used as one of the operations in value.',
                                type: 'string',
                                default: 'parent-name'
                            },
                            value: {
                                description: 'Changes to the material/transform from its base/parent.',
                                oneOf: [
                                    { $ref: '#/definitions/material' },
                                    { $ref: '#/definitions/transform' }
                                ],
                                default: [] // can be empty
                            }
                        },
                        required: ['define', 'value']
                    }
                }
            }
        }
    ]
})

// Create the code editor in the DOM
const editor = monaco.editor.create(document.getElementById('editor'), {
    value: `# the camera
# ======================================================
- add: camera
  width: 100
  height: 100
  field-of-view: 0.785
  from: [-6, 6, -10]
  to: [6, 0, 6]
  up: [-0.45, 1, 0]
# ======================================================
# light sources
# ======================================================
- add: light
  at: [50, 100, -50]
  intensity: [1, 1, 1]
# an optional second light for additional illumination
- add: light
  at: [-400, 50, -10]
  intensity: [0.2, 0.2, 0.2]
# ======================================================
# define some constants to avoid duplication
# ======================================================
- define: white-material
  value:
    color: [1, 1, 1]
    diffuse: 0.7
    ambient: 0.1
    specular: 0.0
    reflective: 0.1
- define: blue-material
  extend: white-material
  value:
    color: [0.537, 0.831, 0.914]
- define: red-material
  extend: white-material
  value:
    color: [0.941, 0.322, 0.388]
- define: purple-material
  extend: white-material
  value:
    color: [0.373, 0.404, 0.550]
- define: standard-transform
  value:
    - [translate, 1, -1, 1]
    - [scale, 0.5, 0.5, 0.5]
- define: large-object
  value:
    - standard-transform
    - [scale, 3.5, 3.5, 3.5]
- define: medium-object
  value:
    - standard-transform
    - [scale, 3, 3, 3]
- define: small-object
  value:
    - standard-transform
    - [scale, 2, 2, 2]
# ======================================================
# a white backdrop for the scene
# ======================================================
- add: plane
  material:
    color: [1, 1, 1]
    ambient: 1
    diffuse: 0
    specular: 0
  transform:
    - [rotate-x, 1.5707963267948966] # pi/2
    - [translate, 0, 0, 500]
# ======================================================
# describe the elements of the scene
# ======================================================
- add: sphere
  material:
    color: [0.373, 0.404, 0.550]
    diffuse: 0.2
    ambient: 0.0
    specular: 1.0
    shininess: 200
    reflective: 0.7
    transparency: 0.7
    refractive-index: 1.5
  transform:
    - large-object
- add: cube
  material: white-material
  transform:
    - medium-object
    - [translate, 4, 0, 0]
- add: cube
  material: blue-material
  transform:
    - large-object
    - [translate, 8.5, 1.5, -0.5]
- add: cube
  material: red-material
  transform:
    - large-object
    - [translate, 0, 0, 4]
- add: cube
  material: white-material
  transform:
    - small-object
    - [translate, 4, 0, 4]
- add: cube
  material: purple-material
  transform:
    - medium-object
    - [translate, 7.5, 0.5, 4]
- add: cube
  material: white-material
  transform:
    - medium-object
    - [translate, -0.25, 0.25, 8]
- add: cube
  material: blue-material
  transform:
    - large-object
    - [translate, 4, 1, 7.5]
- add: cube
  material: red-material
  transform:
    - medium-object
    - [translate, 10, 2, 7.5]
- add: cube
  material: white-material
  transform:
    - small-object
    - [translate, 8, 2, 12]
- add: cube
  material: white-material
  transform:
    - small-object
    - [translate, 20, 1, 9]
- add: cube
  material: blue-material
  transform:
    - large-object
    - [translate, -0.5, -5, 0.25]
- add: cube
  material: red-material
  transform:
    - large-object
    - [translate, 4, -4, 0]
- add: cube
  material: white-material
  transform:
    - large-object
    - [translate, 8.5, -4, 0]
- add: cube
  material: white-material
  transform:
    - large-object
    - [translate, 0, -4, 4]
- add: cube
  material: purple-material
  transform:
    - large-object
    - [translate, -0.5, -4.5, 8]
- add: cube
  material: white-material
  transform:
    - large-object
    - [translate, 0, -8, 4]
- add: cube
  material: white-material
  transform:
    - large-object
    - [translate, -0.5, -8.5, 8]
`,
    language: 'yaml',
    theme: 'vs-dark',
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
})


// Variables related to the image display / persistent data
let scaleImage = true;
let savedImage = null;
let debugLetterbox = true;
const editorDiv = document.getElementById("editor");
const canvas = document.getElementById("output");



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


// This function is similar to the end of render, we're just changing how to show the image on the canvas
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





document.getElementById('download-btn').addEventListener('click', () => {
    // We'll actually use the saved image canvas for downloading because it contains the rendered image
    const link = document.createElement('a')
    link.download = 'scene.png'
    link.href = savedImage.toDataURL('image/png')
    link.click()
})

document.getElementById('toggle-fit').addEventListener('click', () => {
    scaleImage = !scaleImage
    drawOutputCanvas()
})

// Match canvas CSS size to editor size
function resizeCanvasCSS() {
    const rect = editorDiv.getBoundingClientRect();
    canvas.style.width = rect.width + "px";
    canvas.style.height = rect.height + "px";
    editor.layout();
}

window.addEventListener('resize', () => {
    // Resize monaco and the canvas
    editor.layout()
    resizeCanvasCSS()
    drawOutputCanvas()
})


// On page load
resizeCanvasCSS()
