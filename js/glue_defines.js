
// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
var INITIAL_WASM_MEMORY_PAGE_COUNT = 7;
var WASM_FILE_PATH = "MapEditor.wasm"

var ArenaName_MainHeap  = 0;
var ArenaName_TempArena = 1;
var ArenaName_NumNames  = 2;

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
var wasmMemory = null;
var canvasContextGl = null;
var webglObjects = {
	programs: [ null ],
	uniforms: [ null ],
	shaders: [ null ],
	vertBuffers: [ null ],
	vertArrays: [ null ],
	frameBuffers: [ null ],
	textures: [ null ],
}
var wasmModule = null;
var mousePositionX = 0;
var mousePositionY = 0;
