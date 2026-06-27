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
    value: '# Write your scene YAML here\n',
    language: 'yaml',
    theme: 'vs-dark',
    autoIndent: 'full',
    formatOnType: true,
    formatOnPaste: true,
    quickSuggestionsDelay: 0,
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

document.getElementById('render-btn').addEventListener('click', () => {
    const yaml = editor.getValue()
    const maxDepth = 5

    Module.ccall('render', null, ['string', 'number'], [yaml, maxDepth])

    const error = Module.ccall('get_last_error', 'string', [], [])
    console.log('error:', error)

    const width = Module.ccall('get_width', 'number', [], [])
    const height = Module.ccall('get_height', 'number', [], [])
    console.log('width:', width, 'height:', height)

    const bufferSize = width * height * 4
    const bufferPtr = Module._malloc(bufferSize)
    console.log('bufferPtr:', bufferPtr, 'bufferSize:', bufferSize)

    Module.ccall('copy_to_buffer', null, ['number'], [bufferPtr])

    const pixels = new Uint8ClampedArray(Module.HEAPU8.buffer, bufferPtr, bufferSize)
    
    console.log('first few pixels:', pixels[0], pixels[1], pixels[2], pixels[3])
    const imageData = new ImageData(pixels, width, height)
    const outputCanvas = document.getElementById('output')
    outputCanvas.width = width
    outputCanvas.height = height

    outputCanvas.getContext('2d').putImageData(imageData, 0, 0)
    Module._free(bufferPtr)

    document.getElementById('download-btn').style.display = 'inline'
})

document.getElementById('download-btn').addEventListener('click', () => {
    const canvas = document.getElementById('output')
    const link = document.createElement('a')
    link.download = 'scene.png'
    link.href = canvas.toDataURL('image/png')
    link.click()
})
