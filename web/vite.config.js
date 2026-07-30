import { defineConfig } from 'vite'

export default defineConfig({
  base: '/RayTracer/',
  publicDir: 'public',
  build: {
    sourcemap: false
  },
  optimizeDeps: {
    include: ['path-browserify'],
  },
  resolve: {
    alias: {
      path: 'path-browserify'
    }
  },
  server: {
    headers: {
      'Cross-Origin-Opener-Policy': 'same-origin',
      'Cross-Origin-Embedder-Policy': 'require-corp',
    }
  },
  preview: {
    headers: {
      'Cross-Origin-Opener-Policy': 'same-origin',
      'Cross-Origin-Embedder-Policy': 'require-corp',
    }
  }
})