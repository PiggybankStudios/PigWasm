
// +--------------------------------------------------------------+
// |                          Intrinsics                          |
// +--------------------------------------------------------------+
function js_sinf(value)
{
	return Math.sin(value);
}
function js_cosf(value)
{
	return Math.cos(value);
}
function js_roundf(value)
{
	return Math.round(value);
}
function js_ldexp(value, exponent)
{
	//TODO: Maybe we need to care about inaccuracy here?? Check https://blog.codefrau.net/2014/08/deconstructing-floats-frexp-and-ldexp.html
	return value * Math.pow(2, exponent);
}
function js_pow(base, exponent)
{
	return Math.pow(base, exponent);
}

apiFuncs_intrinsics = {
	sinf:   js_sinf,
	cosf:   js_cosf,
	roundf: js_roundf,
	ldexp:  js_ldexp,
	pow:    js_pow,
};

// +--------------------------------------------------------------+
// |                       Custom Functions                       |
// +--------------------------------------------------------------+
function RequestMoreMemoryPages(numPages)
{
	// console.log("Memory growing by " + numPages + " pages");
	wasmMemory.grow(numPages);
}

function PrintoutStack()
{
	console.trace();
}

function DebugOutput(level, messagePtr)
{
	if (level >= 6) //DbgLevel_Error
	{
		console.error(wasmPntrToJsString(messagePtr));
	}
	else if (level == 5) //DbgLevel_Warn
	{
		console.warn(wasmPntrToJsString(messagePtr));
	}
	else
	{
		var colorString = "";
		if (level == 0) { colorString = "color: #AfAfA2;"; } //DbgLevel_Debug  MonokaiGray1
		if (level == 1) { colorString = "";                } //DbgLevel_Log    Default Color
		if (level == 2) { colorString = "color: #A6E22E;"; } //DbgLevel_Info   MonokaiGreen
		if (level == 3) { colorString = "color: #AE81FF;"; } //DbgLevel_Notify MonokaiPurple
		if (level == 4) { colorString = "color: #66D9EF;"; } //DbgLevel_Other  MonokaiBlue
		console.log("%c" + wasmPntrToJsString(messagePtr), colorString);
	}
}

function GetCanvasSize(widthOutPntr, heightOutPntr)
{
	let pixelRatio = window.devicePixelRatio;
	let canvasWidth = canvas.width / pixelRatio;
	let canvasHeight = canvas.height / pixelRatio;
	// console.log("Canvas size: " + canvasWidth + "x" + canvasHeight + " (ratio " + pixelRatio + ")");
	WritePntr_R32(widthOutPntr, canvasWidth);
	WritePntr_R32(heightOutPntr, canvasHeight);
}

function GetMousePosition(xPosOutPntr, yPosOutPntr)
{
	WritePntr_R32(xPosOutPntr, mousePositionX);
	WritePntr_R32(yPosOutPntr, mousePositionY);
}

function RequestFileAsync(requestId, filePathPntr)
{
	let filePath = wasmPntrToJsString(filePathPntr);
	console.log("RequestFileAsync(" + requestId + ", " + filePath + ")");
	fetch("http://localhost:8000/" + filePath, { cache: "no-cache" })
	.then(data => data.blob())
	.then(blob => blob.arrayBuffer())
	.then(resultBuffer =>
	{
		console.log(resultBuffer);
		let bufferU8 = new Uint8Array(resultBuffer);
		let spacePntr = wasmModule.exports.AllocateMemory(ArenaName_MainHeap, resultBuffer.byteLength);
		// console.log("Allocated at " + spacePntr);
		let buf = new Uint8Array(wasmMemory.buffer);
		for (let bIndex = 0; bIndex < resultBuffer.byteLength; bIndex++)
		{
			buf[spacePntr + bIndex] = bufferU8[bIndex];
		}
		wasmModule.exports.HandleFileReady(requestId, resultBuffer.byteLength, spacePntr);
		wasmModule.exports.FreeMemory(ArenaName_MainHeap, spacePntr, resultBuffer.byteLength);
	});
}

function TestFunction()
{
	return jsStringToWasmPntr(ArenaName_MainHeap, "Hello from Javascript!");
}

apiFuncs_custom = {
	RequestMoreMemoryPages: RequestMoreMemoryPages,
	PrintoutStack: PrintoutStack,
	DebugOutput: DebugOutput,
	GetCanvasSize: GetCanvasSize,
	GetMousePosition: GetMousePosition,
	RequestFileAsync: RequestFileAsync,
	TestFunction: TestFunction,
};

// +--------------------------------------------------------------+
// |                       OpenGL Functions                       |
// +--------------------------------------------------------------+
function glGetError() //returns i32
{
	return canvasContextGl.getError();
}
function glGetBooleanv(pname, params)
{
	var value = canvasContextGl.getParameter(pname);
	WritePntr_Bool(params, value);
}
function glGetShaderiv(shaderId, pname, params)
{
	console.assert(shaderId < webglObjects.shaders.length, "Invalid shader ID passed to glGetShaderiv");
	var value = canvasContextGl.getShaderParameter(webglObjects.shaders[shaderId], pname);
	WritePntr_I32(params, value);
}
function glGetProgramiv(programId, pname, params)
{
	if (!verifyParameter(verifyProgramId(programId), "glGetProgramiv", "programId", programId)) { return; }
	var value = canvasContextGl.getProgramParameter(webglObjects.programs[programId], pname);
	// console.log("Programiv: " + value);
	WritePntr_I32(params, value);
}
function glGetShaderInfoLog(shaderId, maxLength, lengthOutPntr, infoLogPntr)
{
	if (!verifyParameter(verifyShaderId(shaderId), "glGetShaderInfoLog", "shaderId", shaderId)) { return; }
	var logString = canvasContextGl.getShaderInfoLog(webglObjects.shaders[shaderId]);
	if (logString.length > maxLength) { logString = logString.substring(0, maxLength); }
	// console.log("Shader info log:", logString);
	WritePntr_I32(lengthOutPntr, logString.length);
	writeToWasmCharBuffer(maxLength, infoLogPntr, logString);
}
function glGetProgramInfoLog(programId, maxLength, lengthOutPntr, infoLogPntr)
{
	if (!verifyParameter(verifyProgramId(programId), "glGetProgramInfoLog", "programId", programId)) { return; }
	var logString = canvasContextGl.getProgramInfoLog(webglObjects.programs[programId]);
	if (logString.length > maxLength) { logString = logString.substring(0, maxLength); }
	// console.log("Program info log:", logString);
	WritePntr_I32(lengthOutPntr, logString.length);
	writeToWasmCharBuffer(maxLength, infoLogPntr, logString);
}
function glGetAttribLocation(programId, namePntr) //returns GLint
{
	if (!verifyParameter(verifyProgramId(programId), "glGetAttribLocation", "programId", programId)) { return; }
	let result = canvasContextGl.getAttribLocation(webglObjects.programs[programId], wasmPntrToJsString(namePntr));
	return result;
}
function glGetUniformLocation(programId, namePntr) //returns GLint
{
	if (!verifyParameter(verifyProgramId(programId), "glGetUniformLocation", "programId", programId)) { return; }
	let uniformLocationObj = canvasContextGl.getUniformLocation(webglObjects.programs[programId], wasmPntrToJsString(namePntr));
	if (uniformLocationObj != null)
	{
		let locationId = webglObjects.uniforms.length;
		webglObjects.uniforms.push(uniformLocationObj);
		return locationId;
	}
	else
	{
		return -1;
	}
}
function glGetTextureSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, bufSize, pixels)
{
	console.error("glGetTextureSubImage is unimplemented!"); //TODO: Implement me!
}
function glCheckFramebufferStatus(target) //returns GLenum
{
	console.error("glCheckFramebufferStatus is unimplemented!"); //TODO: Implement me!
}
function glEnable(cap)
{
	canvasContextGl.enable(cap);
}
function glDisable(cap)
{
	canvasContextGl.disable(cap);
}
function glBlendFunc(sfactor, dfactor)
{
	console.error("glBlendFunc is unimplemented!"); //TODO: Implement me!
}
function glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha)
{
	console.error("glBlendFuncSeparate is unimplemented!"); //TODO: Implement me!
}
function glDepthFunc(func)
{
	console.error("glDepthFunc is unimplemented!"); //TODO: Implement me!
}
function glAlphaFunc(func, ref)
{
	console.error("glAlphaFunc is unimplemented!"); //TODO: Implement me!
}
function glFrontFace(mode)
{
	console.error("glFrontFace is unimplemented!"); //TODO: Implement me!
}
function glLineWidth(width)
{
	console.error("glLineWidth is unimplemented!"); //TODO: Implement me!
}
function glGenFramebuffer()
{
	console.error("glGenFramebuffer is unimplemented!"); //TODO: Implement me!
}
function glGenVertexArray()
{
	let vao = canvasContextGl.createVertexArray();
	let vaoId = webglObjects.vertArrays.length;
	webglObjects.vertArrays.push(vao);
	return vaoId;
}
function glGenTexture()
{
	console.error("glGenTexture is unimplemented!"); //TODO: Implement me!
}
function glGenBuffer()
{
	let buffer = canvasContextGl.createBuffer();
	let bufferId = webglObjects.vertBuffers.length;
	webglObjects.vertBuffers.push(buffer);
	return bufferId;
}
function glCreateShader(shaderType) //returns GLuint
{
	let shader = canvasContextGl.createShader(shaderType);
	let shaderId = webglObjects.shaders.length;
	webglObjects.shaders.push(shader);
	return shaderId;
}
function glCreateProgram() //returns GLuint
{
	let program = canvasContextGl.createProgram();
	let programId = webglObjects.programs.length;
	webglObjects.programs.push(program);
	return programId;
}
function glDeleteFramebuffer(frameBufferId)
{
	console.error("glDeleteFramebuffer is unimplemented!"); //TODO: Implement me!
}
function glDeleteTexture(textureId)
{
	console.error("glDeleteTexture is unimplemented!"); //TODO: Implement me!
}
function glDeleteShader(shaderId)
{
	console.assert(shaderId < webglObjects.shaders.length, "Invalid shader ID passed to glDeleteShader");
	canvasContextGl.deleteShader(webglObjects.shaders[shaderId]);
	webglObjects.shaders[shaderId] = null;
}
function glDeleteProgram(programId)
{
	if (!verifyParameter(verifyProgramId(programId), "glDeleteProgram", "programId", programId)) { return; }
	canvasContextGl.deleteProgram(webglObjects.programs[programId]);
	webglObjects.programs[programId] = null;
}
function glDeleteBuffer(bufferId)
{
	if (!verifyParameter(verifyVertBufferId(bufferId), "glDeleteBuffer", "bufferId", bufferId)) { return; }
	canvasContextGl.deleteBuffer(webglObjects.vertBuffers[bufferId]);
	webglObjects.vertBuffers[bufferId] = null;
}
function glBindFramebuffer(target, framebuffer)
{
	console.error("glBindFramebuffer is unimplemented!"); //TODO: Implement me!
}
function glBindVertexArray(vaoId)
{
	if (!verifyParameter(verifyVertArrayId(vaoId), "glBindVertexArray", "vaoId", vaoId)) { return; }
	canvasContextGl.bindVertexArray(webglObjects.vertArrays[vaoId]);
}
function glBindTexture(target, texture)
{
	console.error("glBindTexture is unimplemented!"); //TODO: Implement me!
}
function glBindBuffer(target, bufferId)
{
	if (!verifyParameter(verifyVertBufferId(bufferId), "glBindBuffer", "bufferId", bufferId)) { return; }
	canvasContextGl.bindBuffer(target, webglObjects.vertBuffers[bufferId]);
}
function glUseProgram(programId)
{
	if (!verifyParameter(verifyProgramId(programId), "glUseProgram", "programId", programId)) { return; }
	canvasContextGl.useProgram(webglObjects.programs[programId]);
}
function glTexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations)
{
	console.error("glTexImage2DMultisample is unimplemented!"); //TODO: Implement me!
}
function glTexImage2D(target, level, internalformat, width, height, border, format, type, data)
{
	console.error("glTexImage2D is unimplemented!"); //TODO: Implement me!
}
function glTexParameteri(target, pname, param)
{
	console.error("glTexParameteri is unimplemented!"); //TODO: Implement me!
}
function glTexParameteriv(target, pname, params)
{
	console.error("glTexParameteriv is unimplemented!"); //TODO: Implement me!
}
function glEnableVertexAttribArray(index)
{
	canvasContextGl.enableVertexAttribArray(index);
}
function glActiveTexture(texture)
{
	console.error("glActiveTexture is unimplemented!"); //TODO: Implement me!
}
function glVertexAttribPointer(index, size, type, normalized, stride, pointer)
{
	canvasContextGl.vertexAttribPointer(index, size, type, normalized, stride, pointer);
}
function glShaderSource(shaderId, string)
{
	console.assert(shaderId < webglObjects.shaders.length, "Invalid shader ID passed to glShaderSource");
	// console.log("Shader source:" + wasmPntrToJsString(string));
	canvasContextGl.shaderSource(webglObjects.shaders[shaderId], wasmPntrToJsString(string));
}
function glCompileShader(shaderId)
{
	console.assert(shaderId < webglObjects.shaders.length, "Invalid shader ID passed to glCompileShader");
	canvasContextGl.compileShader(webglObjects.shaders[shaderId]);
}
function glAttachShader(programId, shaderId)
{
	if (!verifyParameter(verifyProgramId(programId), "glAttachShader", "programId", programId)) { return; }
	console.assert(shaderId < webglObjects.shaders.length, "Invalid shader ID passed to glAttachShader");
	canvasContextGl.attachShader(webglObjects.programs[programId], webglObjects.shaders[shaderId]);
}
function glLinkProgram(programId)
{
	if (!verifyParameter(verifyProgramId(programId), "glLinkProgram", "programId", programId)) { return; }
	canvasContextGl.linkProgram(webglObjects.programs[programId]);
}
function glGenerateMipmap(target)
{
	console.error("glGenerateMipmap is unimplemented!"); //TODO: Implement me!
}
function glBufferData(target, size, dataPntr, usage)
{
	let dataBuf = new Uint8Array(wasmMemory.buffer, dataPntr, size);
	canvasContextGl.bufferData(target, dataBuf, usage, 0);
}
function glBufferSubData(target, offset, size, data)
{
	console.error("glBufferSubData is unimplemented!"); //TODO: Implement me!
}
function glRenderbufferStorageMultisample(target, samples, internalformat, width, height)
{
	console.error("glRenderbufferStorageMultisample is unimplemented!"); //TODO: Implement me!
}
function glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer)
{
	console.error("glFramebufferRenderbuffer is unimplemented!"); //TODO: Implement me!
}
function glFramebufferTexture2D(target, attachment, textarget, texture, level)
{
	console.error("glFramebufferTexture2D is unimplemented!"); //TODO: Implement me!
}
function glUniform1i(locationId, v0)
{
	if (!verifyParameter(verifyUniformLocationId(locationId), "glUniform1i", "locationId", locationId)) { return; }
	canvasContextGl.uniform1i(webglObjects.uniforms[locationId], v0);
}
function glUniform1f(locationId, v0)
{
	if (!verifyParameter(verifyUniformLocationId(locationId), "glUniform1f", "locationId", locationId)) { return; }
	canvasContextGl.uniform1f(webglObjects.uniforms[locationId], v0);
}
function glUniform2f(locationId, v0, v1)
{
	if (!verifyParameter(verifyUniformLocationId(locationId), "glUniform2f", "locationId", locationId)) { return; }
	canvasContextGl.uniform2f(webglObjects.uniforms[locationId], v0, v1);
}
function glUniform3f(locationId, v0, v1, v2)
{
	if (!verifyParameter(verifyUniformLocationId(locationId), "glUniform3f", "locationId", locationId)) { return; }
	canvasContextGl.uniform3f(webglObjects.uniforms[locationId], v0, v1, v2);
}
function glUniform4f(locationId, v0, v1, v2, v3)
{
	if (!verifyParameter(verifyUniformLocationId(locationId), "glUniform4f", "locationId", locationId)) { return; }
	canvasContextGl.uniform4f(webglObjects.uniforms[locationId], v0, v1, v2, v3);
}
function glUniformMatrix4fv(locationId, count, transpose, valuePntr)
{
	if (!verifyParameter(verifyUniformLocationId(locationId), "glUniformMatrix4fv", "locationId", locationId)) { return; }
	let matrixArray = new Float32Array(wasmMemory.buffer, valuePntr, count * (4 * 4)); //4 * 4 for 4x4 size matrix
	canvasContextGl.uniformMatrix4fv(webglObjects.uniforms[locationId], transpose, matrixArray);
}
function glViewport(x, y, width, height)
{
	canvasContextGl.viewport(x, y, width, height);
}
function glClearColor(red, green, blue, alpha)
{
	canvasContextGl.clearColor(red, green, blue, alpha);
}
function glClearDepth(depth)
{
	canvasContextGl.clearDepth(depth);
}
function glClearStencil(s)
{
	canvasContextGl.clearStencil(s);
}
function glClear(mask)
{
	canvasContextGl.clear(mask);
}
function glDrawArrays(mode, first, count)
{
	canvasContextGl.drawArrays(mode, first, count);
}
function glDrawBuffers(n, bufs)
{
	let buffers = [];
	for (let bIndex = 0; bIndex < n; bIndex++)
	{
		buffers.push(ReadPntr_U32(bufs + bIndex*4));
	}
	canvasContextGl.drawBuffers(buffers);
}

apiFuncs_opengl = {
	glGetError:                       glGetError,
	glGetBooleanv:                    glGetBooleanv,
	glGetShaderiv:                    glGetShaderiv,
	glGetProgramiv:                   glGetProgramiv,
	glGetShaderInfoLog:               glGetShaderInfoLog,
	glGetProgramInfoLog:              glGetProgramInfoLog,
	glGetAttribLocation:              glGetAttribLocation,
	glGetUniformLocation:             glGetUniformLocation,
	glGetTextureSubImage:             glGetTextureSubImage,
	glCheckFramebufferStatus:         glCheckFramebufferStatus,
	glEnable:                         glEnable,
	glDisable:                        glDisable,
	glBlendFunc:                      glBlendFunc,
	glBlendFuncSeparate:              glBlendFuncSeparate,
	glDepthFunc:                      glDepthFunc,
	glAlphaFunc:                      glAlphaFunc,
	glFrontFace:                      glFrontFace,
	glLineWidth:                      glLineWidth,
	glGenFramebuffer:                 glGenFramebuffer,
	glGenVertexArray:                 glGenVertexArray,
	glGenTexture:                     glGenTexture,
	glGenBuffer:                      glGenBuffer,
	glCreateShader:                   glCreateShader,
	glCreateProgram:                  glCreateProgram,
	glDeleteFramebuffer:              glDeleteFramebuffer,
	glDeleteTexture:                  glDeleteTexture,
	glDeleteShader:                   glDeleteShader,
	glDeleteProgram:                  glDeleteProgram,
	glDeleteBuffer:                   glDeleteBuffer,
	glBindFramebuffer:                glBindFramebuffer,
	glBindVertexArray:                glBindVertexArray,
	glBindTexture:                    glBindTexture,
	glBindBuffer:                     glBindBuffer,
	glUseProgram:                     glUseProgram,
	glTexImage2DMultisample:          glTexImage2DMultisample,
	glTexImage2D:                     glTexImage2D,
	glTexParameteri:                  glTexParameteri,
	glTexParameteriv:                 glTexParameteriv,
	glEnableVertexAttribArray:        glEnableVertexAttribArray,
	glActiveTexture:                  glActiveTexture,
	glVertexAttribPointer:            glVertexAttribPointer,
	glShaderSource:                   glShaderSource,
	glCompileShader:                  glCompileShader,
	glAttachShader:                   glAttachShader,
	glLinkProgram:                    glLinkProgram,
	glGenerateMipmap:                 glGenerateMipmap,
	glBufferData:                     glBufferData,
	glBufferSubData:                  glBufferSubData,
	glRenderbufferStorageMultisample: glRenderbufferStorageMultisample,
	glFramebufferRenderbuffer:        glFramebufferRenderbuffer,
	glFramebufferTexture2D:           glFramebufferTexture2D,
	glUniform1i:                      glUniform1i,
	glUniform1f:                      glUniform1f,
	glUniform2f:                      glUniform2f,
	glUniform3f:                      glUniform3f,
	glUniform4f:                      glUniform4f,
	glUniformMatrix4fv:               glUniformMatrix4fv,
	glViewport:                       glViewport,
	glClearColor:                     glClearColor,
	glClearDepth:                     glClearDepth,
	glClearStencil:                   glClearStencil,
	glClear:                          glClear,
	glDrawArrays:                     glDrawArrays,
	glDrawBuffers:                    glDrawBuffers,
};
