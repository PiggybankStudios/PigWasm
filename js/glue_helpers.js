
async function loadWasmModule(filePath, environment)
{
	// console.log("Loading " + filePath + "...");
	let result = null;
	try
	{
		const fetchPromise = fetch(filePath);
		const wasmModule = await WebAssembly.instantiateStreaming(
			fetchPromise,
			{ env: environment }
		);
		result = wasmModule.instance;
		// console.log("Loaded module exports:", result.exports);
	}
	catch (exception)
	{
		console.error("Failed to load WASM module from \"" + filePath + "\":", exception);
	}
	return result;
}

// +--------------------------------------------------------------+
// |                     Wasm Data Marshaling                     |
// +--------------------------------------------------------------+
//TODO: Add support for utf-8 encoding
function wasmPntrToJsString(ptr)
{
	const codes = [];
	const buf = new Uint8Array(wasmMemory.buffer);
	
	let cIndex = 0;
	while (true)
	{
		const char = buf[ptr + cIndex];
		if (!char) { break; }
		codes.push(char);
		cIndex++;
	}
	
	//TODO: Can we do something else? If we do our own UTF-8 parsing maybe?
	return String.fromCharCode(...codes);
}

function jsStringToWasmPntr(engine, jsString)
{
	let allocSize = jsString.length+1;
	let result = engine.exports.AllocWasmMemory(allocSize);
	if (result != 0)
	{
		let buf = new Uint8Array(wasmMemory.buffer);
		for (var cIndex = 0; cIndex < jsString.length; cIndex++)
		{
			buf[result + cIndex] = jsString[cIndex];
		}
		buf[result + jsString.length] = '\0';
	}
	return result;
}

function writeToWasmCharBuffer(bufferLength, bufferPntr, stringToWrite)
{
	console.assert(typeof(bufferLength)  == "number", "Invalid type for bufferLength passed to writeToWasmCharBuffer");
	console.assert(typeof(bufferPntr)    == "number", "Invalid type for bufferPntr passed to writeToWasmCharBuffer");
	console.assert(typeof(stringToWrite) == "string", "Invalid type for stringToWrite passed to writeToWasmCharBuffer");
	let encodedStr = new TextEncoder().encode(stringToWrite);
	let buf = new Uint8Array(wasmMemory.buffer);
	for (let cIndex = 0; cIndex < encodedStr.length && cIndex < bufferLength-1; cIndex++)
	{
		buf[bufferPntr + cIndex] = encodedStr[cIndex];
	}
	buf[bufferPntr + Math.min(encodedStr.length, bufferLength-1)] = 0;
}

// +--------------------------------------------------------------+
// |               WritePntr and ReadPntr Functions               |
// +--------------------------------------------------------------+
function WritePntr_Bool(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	if (value)
	{
		wasmDataView.setInt8(pntr, 1);
	}
	else
	{
		wasmDataView.setInt8(pntr, 0);
	}
}
function WritePntr_U8(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	wasmDataView.setUint8(pntr, value);
}
function WritePntr_U16(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	wasmDataView.setUint16(pntr, value, true);
}
function WritePntr_U32(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	wasmDataView.setUint32(pntr, value, true);
}
function WritePntr_U64(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	wasmDataView.setUint64(pntr, value, true);
}
function WritePntr_I8(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	wasmDataView.setInt8(pntr, value);
}
function WritePntr_I16(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	wasmDataView.setInt16(pntr, value, true);
}
function WritePntr_I32(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	wasmDataView.setInt32(pntr, value, true);
}
function WritePntr_I64(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	wasmDataView.setInt64(pntr, value, true);
}
function WritePntr_R32(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	wasmDataView.setFloat32(pntr, value, true);
}
function WritePntr_R64(pntr, value)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	wasmDataView.setFloat64(pntr, value, true);
}

function ReadPntr_Bool(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	let intValue = wasmDataView.getUint8(pntr);
	return (intValue != 0);
}
function ReadPntr_U8(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	return wasmDataView.getUint8(pntr);
}
function ReadPntr_U16(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	return wasmDataView.getUint16(pntr, true);
}
function ReadPntr_U32(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	return wasmDataView.getUint32(pntr, true);
}
function ReadPntr_U64(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	return wasmDataView.getUint64(pntr, true);
}
function ReadPntr_I8(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	return wasmDataView.getInt8(pntr);
}
function ReadPntr_I16(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	return wasmDataView.getInt16(pntr, true);
}
function ReadPntr_I32(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	return wasmDataView.getInt32(pntr, true);
}
function ReadPntr_I64(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	return wasmDataView.getInt64(pntr, true);
}
function ReadPntr_R32(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	return wasmDataView.getFloat32(pntr, true);
}
function ReadPntr_R64(pntr)
{
	let wasmDataView = new DataView(wasmMemory.buffer);
	return wasmDataView.getFloat64(pntr, true);
}

// +--------------------------------------------------------------+
// |                        Verify Helpers                        |
// +--------------------------------------------------------------+
function verifyProgramId(programId)
{
	if (typeof(programId) != "number") { return "ProgramId is not a number!"; }
	if (programId == 0) { return "ProgramId is 0!"; }
	if (webglObjects == null || webglObjects.programs == null) { return "Programs array has not been initialized yet!"; }
	if (programId >= webglObjects.programs.length) { return "ProgramId is too high!"; }
	if (webglObjects.programs[programId] == null) { return "ProgramId is for a destroyed program!"; }
	return null;
}
function verifyShaderId(shaderId)
{
	if (typeof(shaderId) != "number") { return "ShaderId is not a number!"; }
	if (shaderId == 0) { return "ShaderId is 0!"; }
	if (webglObjects == null || webglObjects.shaders == null) { return "Shaders array has not been initialized yet!"; }
	if (shaderId >= webglObjects.shaders.length) { return "ShaderId is too high!"; }
	if (webglObjects.shaders[shaderId] == null) { return "ShaderId is for a destroyed shader!"; }
	return null;
}
function verifyUniformLocationId(locationId)
{
	if (typeof(locationId) != "number") { return "UniformLocationId is not a number!"; }
	if (locationId == 0) { return "UniformLocationId is 0!"; }
	if (webglObjects == null || webglObjects.uniforms == null) { return "Uniforms array has not been initialized yet!"; }
	if (locationId >= webglObjects.uniforms.length) { return "UniformLocationId is too high!"; }
	if (webglObjects.uniforms[locationId] == null) { return "UniformLocationId is for a destroyed uniform!"; }
	return null;
}
function verifyVertBufferId(vertBufferId)
{
	if (typeof(vertBufferId) != "number") { return "VertBufferId is not a number!"; }
	if (vertBufferId == 0) { return "VertBufferId is 0!"; }
	if (webglObjects == null || webglObjects.vertBuffers == null) { return "VertBuffers array has not been initialized yet!"; }
	if (vertBufferId >= webglObjects.vertBuffers.length) { return "VertBufferId is too high!"; }
	if (webglObjects.vertBuffers[vertBufferId] == null) { return "VertBufferId is for a destroyed vertBuffer!"; }
	return null;
}
function verifyVertArrayId(vertArrayId)
{
	if (typeof(vertArrayId) != "number") { return "VertArrayId is not a number!"; }
	if (vertArrayId == 0) { return "VertArrayId is 0!"; }
	if (webglObjects == null || webglObjects.vertArrays == null) { return "VertArrays array has not been initialized yet!"; }
	if (vertArrayId >= webglObjects.vertArrays.length) { return "VertArrayId is too high!"; }
	if (webglObjects.vertArrays[vertArrayId] == null) { return "VertArrayId is for a destroyed array!"; }
	return null;
}
function verifyFrameBufferId(frameBufferId)
{
	if (typeof(frameBufferId) != "number") { return "FrameBufferId is not a number!"; }
	if (frameBufferId == 0) { return "FrameBufferId is 0!"; }
	if (webglObjects == null || webglObjects.frameBuffers == null) { return "FrameBuffers array has not been initialized yet!"; }
	if (frameBufferId >= webglObjects.frameBuffers.length) { return "FrameBufferId is too high!"; }
	if (webglObjects.frameBuffers[frameBufferId] == null) { return "FrameBufferId is for a destroyed frameBuffer!"; }
	return null;
}
function verifyTextureId(textureId)
{
	if (typeof(textureId) != "number") { return "TextureId is not a number!"; }
	if (textureId == 0) { return "TextureId is 0!"; }
	if (webglObjects == null || webglObjects.textures == null) { return "Textures array has not been initialized yet!"; }
	if (textureId >= webglObjects.textures.length) { return "TextureId is too high!"; }
	if (webglObjects.textures[textureId] == null) { return "TextureId is for a destroyed texture!"; }
	return null;
}

function verifyParameter(verifyResult, functionName, parameterName, parameterValue)
{
	if (verifyResult == null) { return true; }
	console.error("Invalid argument \"" + parameterName + "\" passed to " + functionName + ": " + verifyResult);
	console.error("Argument value: " + parameterValue);
	return false;
}
