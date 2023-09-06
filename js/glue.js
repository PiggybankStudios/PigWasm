
async function initialize()
{
	canvas = document.getElementsByTagName("canvas")[0];
	// console.log(canvas);
	
	// set the display size of the canvas.
	canvas.style.width = DesiredCanvasWidth + "px";
	canvas.style.height = DesiredCanvasHeight + "px";
	
	// set the size of the drawingBuffer
	var devicePixelRatio = window.devicePixelRatio || 1;
	canvas.width = DesiredCanvasWidth * devicePixelRatio;
	canvas.height = DesiredCanvasHeight * devicePixelRatio;
	
	// canvasContainer = document.getElementById("canvas_container");
	// console.assert(canvasContainer != null, "Couldn't find canvas container DOM element!");
	
	canvasContextGl = canvas.getContext("webgl2");
	if (canvasContextGl === null) { console.error("Unable to initialize WebGL render context. Your browser or machine may not support it :("); return; }
	console.log(canvasContextGl);
	
	wasmMemory = new WebAssembly.Memory({ initial: INITIAL_WASM_MEMORY_PAGE_COUNT });
	let wasmEnvironment =
	{
		memory: wasmMemory,
		...apiFuncs_intrinsics,
		...apiFuncs_custom,
		...apiFuncs_opengl,
	};
	
	// console.log("Before loading wasm module we have " + wasmMemory.buffer.byteLength);
	wasmModule = await loadWasmModule(WASM_FILE_PATH, wasmEnvironment);
	// console.log("After loading wasm module we now have " + wasmMemory.buffer.byteLength);
	// console.log("WasmModule:", wasmModule);
	
	let initializeTimestamp = Math.floor(Date.now() / 1000); //TODO: Should we be worried about this being a 32-bit float?
	wasmModule.exports.Initialize(INITIAL_WASM_MEMORY_PAGE_COUNT, initializeTimestamp);
	
	window.addEventListener("mousemove", function(event)
	{
		let clientBounds = canvas.getBoundingClientRect();
		let pixelRatio = window.devicePixelRatio;
		mousePositionX = Math.round(event.clientX - clientBounds.left) * pixelRatio;
		mousePositionY = Math.round(event.clientY - clientBounds.top) * pixelRatio;
	});
	window.addEventListener("keydown", function(event)
	{
		let key = KeyDownEventStrToKeyEnum(event.code);
		wasmModule.exports.HandleKeyPressOrRelease(key, true);
	});
	window.addEventListener("keyup", function(event)
	{
		let key = KeyDownEventStrToKeyEnum(event.code);
		wasmModule.exports.HandleKeyPressOrRelease(key, false);
	});
	window.addEventListener("mousedown", function(event)
	{
		let mouseBtn = MouseDownEventNumToBtnEnum(event.button);
		wasmModule.exports.HandleMousePressOrRelease(mouseBtn, true);
	});
	window.addEventListener("mouseup", function(event)
	{
		let mouseBtn = MouseDownEventNumToBtnEnum(event.button);
		wasmModule.exports.HandleMousePressOrRelease(mouseBtn, false);
	});
	
	wasmModule.exports.UpdateAndRender(0);
	
	function renderFrame()
	{
		wasmModule.exports.UpdateAndRender(16.6666);
		window.requestAnimationFrame(renderFrame);
	}
	window.requestAnimationFrame(renderFrame);
}

initialize();