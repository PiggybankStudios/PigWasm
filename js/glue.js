
async function initialize()
{
	canvas = document.getElementsByTagName("canvas")[0];
	canvasContainer = document.getElementById("canvas_container");
	console.assert(canvas != null, "Couldn't find canvas DOM element!");
	console.assert(canvasContainer != null, "Couldn't find canvas container DOM element!");
	
	canvasContextGl = canvas.getContext("webgl2");
	if (canvasContextGl === null) { console.error("Unable to initialize WebGL render context. Your browser or machine may not support it :("); return; }
	// console.log(canvasContextGl);
	canvas.addEventListener("mousemove", function(event)
	{
		let clientBounds = canvas.getBoundingClientRect();
		mousePositionX = Math.round(event.clientX - clientBounds.left);
		mousePositionY = Math.round(event.clientY - clientBounds.top);
	});
	
	wasmMemory = new WebAssembly.Memory({ initial: INITIAL_WASM_MEMORY_PAGE_COUNT });
	let wasmEnvironment =
	{
		memory: wasmMemory,
		...apiFuncs_intrinsics,
		...apiFuncs_custom,
		...apiFuncs_opengl,
	};
	
	wasmModule = await loadWasmModule(WASM_FILE_PATH, wasmEnvironment);
	// console.log("WasmModule:", wasmModule);
	
	let initializeTimestamp = Math.floor(Date.now() / 1000); //TODO: Should we be worried about this being a 32-bit float?
	wasmModule.exports.Initialize(INITIAL_WASM_MEMORY_PAGE_COUNT, initializeTimestamp);
	
	window.addEventListener("keydown", function(event)
	{
		if (event.key == " ")
		{
			wasmModule.exports.HandleKeyPress(0);
		}
		else if (event.key == "r")
		{
			wasmModule.exports.HandleKeyPress(1);
		}
		else
		{
			// console.warn("Unknown key name \"" + event.key + "\"");
		}
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