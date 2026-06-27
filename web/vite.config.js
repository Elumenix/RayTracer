import { defineConfig } from 'vite'

export default defineConfig({
  base: '/RayTracer/',
  publicDir: 'public',
  optimizeDeps: {
    include: ['path-browserify'],
  },
  resolve: {
    alias: {
      path: 'path-browserify'
    }
  }
})