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
  }
})