// It seems like a worker needs to be configured for the YAML language or else it won't detect errors
// Workers need to be separate files, which is why the imports for the workerrs aren't directly in the following code
self.MonacoEnvironment = {
    getWorker(_, label) {
        if (label === 'yaml') {
            return new Worker(new URL('./yaml.worker.js', import.meta.url), { type: 'module' })
        }
        return new Worker(new URL('./editor.worker.js', import.meta.url), { type: 'module' })
    }
}

// Get monaco packages
import * as monaco from 'monaco-editor'
import { configureMonacoYaml } from 'monaco-yaml'

// This is an example for the schema for the ray tracer
// This isn't correct or finished, I just needed to make sure that error detection works
configureMonacoYaml(monaco, {
    enableSchemaRequest: false,
    schemas: [{
        uri: 'http://raytracer/scene-schema.json',
        fileMatch: ['*'],
        schema: {
            type: 'object',
            properties: {
                camera: {
                    type: 'object',
                    properties: {
                        fov: { type: 'number' },
                        width: { type: 'integer' },
                        height: { type: 'integer' }
                    },
                    required: ['fov', 'width', 'height']
                },
                shapes: {
                    type: 'array',
                    items: {
                        type: 'object',
                        properties: {
                            type: { enum: ['sphere', 'cube', 'plane'] }
                        }
                    }
                }
            }
        }
    }]
})

// Create the code editor in the DOM
monaco.editor.create(document.getElementById('editor'), {
    value: '# Write your scene YAML here\n',
    language: 'yaml',
    theme: 'vs-dark'
})
