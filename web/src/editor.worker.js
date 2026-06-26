// Workers need to be loaded as separate files so they can start their own threads
// Which is why this is the only thing in this file
// I'm not happy about it either, but this is apparently what works
import 'monaco-editor/esm/vs/editor/editor.worker'
