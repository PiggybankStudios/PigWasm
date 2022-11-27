
// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
var INITIAL_WASM_MEMORY_PAGE_COUNT = 7;
var WASM_FILE_PATH = "MapEditor.wasm"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
var wasmMemory = null;
var canvasContextGl = null;
var webglObjects = {
	programs: [ null ],
	shaders: [ null ],
	vertBuffers: [ null ],
	vertArrays: [ null ],
	frameBuffers: [ null ],
	textures: [ null ],
}
var wasmModule = null;
var mousePositionX = 0;
var mousePositionY = 0;
