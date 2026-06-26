import { defineConfig } from 'vite'

export default defineConfig({
  base: '/RayTracer/',
  optimizeDeps: {
    include: ['path-browserify'],
  },
  resolve: {
    alias: {
      path: 'path-browserify'
    }
  }
})