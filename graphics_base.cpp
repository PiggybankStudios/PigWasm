/*
File:   graphics_base.cpp
Author: Taylor Robbins
Date:   11\23\2022
Description: 
	** Holds all the structures that encapsulate graphics API primitives like Textures, Vertex Buffers, Shaders, etc.
*/

#include "common.h"
#include "graphics_base.h"

#include "vectors.h"
#include "debug.h"
#include "stb/stb_image.h"
#include "file_system.h"
#include "memory.h"

static GLchar parseLogBuffer[1024];


// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
FileLoadingManager_t* fileLoadingManager = nullptr;

// +--------------------------------------------------------------+
// |                      Private Functions                       |
// +--------------------------------------------------------------+
static ShaderLoadContext_t* NewShaderLoadContext(Shader_t* targetShader)
{
	for (u64 cIndex = 0; cIndex < MAX_NUM_CONCURRENT_SHADER_LOADS; cIndex++)
	{
		if (!fileLoadingManager->shaderLoadContexts[cIndex].filled)
		{
			ShaderLoadContext_t* context = &fileLoadingManager->shaderLoadContexts[cIndex];
			ClearPointer(context);
			context->filled = true;
			context->targetShader = targetShader;
			return context;
		}
	}
	return nullptr;
}

static TextureLoadContext_t* NewTextureLoadContext(Texture_t* targetTexture, bool pixelated, bool wrapping)
{
	for (u64 cIndex = 0; cIndex < MAX_NUM_CONCURRENT_TEXTURE_LOADS; cIndex++)
	{
		if (!fileLoadingManager->textureLoadContexts[cIndex].filled)
		{
			TextureLoadContext_t* context = &fileLoadingManager->textureLoadContexts[cIndex];
			ClearPointer(context);
			context->filled = true;
			context->targetTexture = targetTexture;
			context->pixelated = pixelated;
			context->wrapping = wrapping;
			return context;
		}
	}
	return nullptr;
}

static void FileLoadingManagerShaderFileReadyCallback(MyStr_t filePath, u32 fileSize, const void* fileData, void* contextPntr)
{
	ShaderLoadContext_t* context = (ShaderLoadContext_t*)contextPntr;
	NotNull2(context, context->targetShader);
	Assert(context->filled == true);
	CreateShaderFromSingleStr(context->targetShader, NewStr(fileSize, (char*)fileData));
	WriteLine_D("Successfully loaded shader");
	context->filled = false;
}

static void FileLoadingManagerTextureFileReadyCallback(MyStr_t filePath, u32 fileSize, const void* fileData, void* contextPntr)
{
	TextureLoadContext_t* context = (TextureLoadContext_t*)contextPntr;
	NotNull2(context, context->targetTexture);
	Assert(context->filled == true);
	if (CreateTextureFromPngFile(context->targetTexture, fileSize, fileData, context->pixelated, context->wrapping))
	{
		PrintLine_D("Successfully loaded texture (%dx%d)", context->targetTexture->size.x, context->targetTexture->size.y);
	}
	else
	{
		PrintLine_D("Failed to parse texture (%u bytes at \"%.*s\")", fileSize, filePath.length, filePath.pntr);
	}
	context->filled = false;
}

// +--------------------------------------------------------------+
// |                        Init and Free                         |
// +--------------------------------------------------------------+
void InitFileLoadingManager(FileLoadingManager_t* fileLoadingManagerPntr)
{
	NotNull(fileLoadingManagerPntr);
	fileLoadingManager = fileLoadingManagerPntr;
	ClearPointer(fileLoadingManager);
}

// +--------------------------------------------------------------+
// |                            Create                            |
// +--------------------------------------------------------------+
void CreateShader(Shader_t* shaderOut, MyStr_t vertSource, MyStr_t fragSource)
{
	GLsizei parseLogLength = 0;
	GLint linkStatus = 0;
	
	ClearPointer(shaderOut);
	shaderOut->vertId = glCreateShader(GL_VERTEX_SHADER);
	shaderOut->fragId = glCreateShader(GL_FRAGMENT_SHADER);
	shaderOut->glId = glCreateProgram();
	// PrintLine_D("vert %d frag %d program %d", shaderOut->vertId, shaderOut->fragId, shaderOut->glId);
	
	PushMemMark(tempArena);
	MyStr_t nullTermedVertSource = AllocString(tempArena, &vertSource);
	MyStr_t nullTermedFragSource = AllocString(tempArena, &fragSource);
	
	glShaderSource(shaderOut->vertId, nullTermedVertSource.chars);
	glCompileShader(shaderOut->vertId);
	glGetShaderInfoLog(shaderOut->vertId, ArrayCount(parseLogBuffer), &parseLogLength, &parseLogBuffer[0]);
	if (parseLogLength > 0)
	{
		PrintLine_W("Vertex shader compilation had %d char log:\n%s", parseLogLength, &parseLogBuffer[0]);
	}
	
	glShaderSource(shaderOut->fragId, nullTermedFragSource.chars);
	glCompileShader(shaderOut->fragId);
	glGetShaderInfoLog(shaderOut->fragId, ArrayCount(parseLogBuffer), &parseLogLength, &parseLogBuffer[0]);
	if (parseLogLength > 0)
	{
		PrintLine_W("Fragment shader compilation had %d char log:\n%s", parseLogLength, &parseLogBuffer[0]);
	}
	
	glAttachShader(shaderOut->glId, shaderOut->vertId);
	glAttachShader(shaderOut->glId, shaderOut->fragId);
	glLinkProgram(shaderOut->glId);
	
	glGetProgramiv(shaderOut->glId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == 0)
	{
		WriteLine_E("Failed to link the shader program!");
	}
	
	glGetProgramInfoLog(shaderOut->glId, ArrayCount(parseLogBuffer), &parseLogLength, &parseLogBuffer[0]);
	if (parseLogLength > 0)
	{
		PrintLine_W("Shader program linking had %d char log:\n%s", parseLogLength, &parseLogBuffer[0]);
	}
	
	for (u32 aIndex = 0; aIndex < VertexAttribute_NumAttributes; aIndex++)
	{
		VertexAttribute_t attribute = (VertexAttribute_t)(1 << aIndex);
		const char* attributeName = GetVertexAttributeGlslName(attribute);
		GLint attribLocation = glGetAttribLocation(shaderOut->glId, attributeName);
		shaderOut->attributeLocations[aIndex] = attribLocation;
		if (attribLocation >= 0) { FlagSet(shaderOut->attributes, attribute); }
	}
	
	for (u32 uIndex = 0; uIndex < ShaderUniform_NumUniforms; uIndex++)
	{
		ShaderUniform_t uniform = (ShaderUniform_t)uIndex;
		const char* uniformName = GetShaderUniformGlslName(uniform);
		GLint uniformLocation = glGetUniformLocation(shaderOut->glId, uniformName);
		shaderOut->uniformLocations[uIndex] = uniformLocation;
	}
	
	PopMemMark(tempArena);
	shaderOut->isValid = true;
}

void CreateShaderFromSingleStr(Shader_t* shaderOut, MyStr_t shaderSourceFileStr)
{
	u32 dividerIndex = 0;
	bool foundDivider = FindSubstring(shaderSourceFileStr, NewStr(FRAGMENT_VERTEX_DIVIDER_STR), &dividerIndex);
	if (!foundDivider)
	{
		WriteLine_E("Failed to find fragment/vertex divider string in shader file source code!");
		return;
	}
	
	u32 prevNewLineIndex = dividerIndex;
	while (prevNewLineIndex > 0 && shaderSourceFileStr.chars[prevNewLineIndex-1] != '\n') { prevNewLineIndex--; }
	u32 nextNewLineIndex = dividerIndex;
	while (nextNewLineIndex < shaderSourceFileStr.length && shaderSourceFileStr.chars[nextNewLineIndex] != '\n') { nextNewLineIndex++; }
	
	CreateShader(shaderOut, StrSubstring(&shaderSourceFileStr, 0, prevNewLineIndex), StrSubstring(&shaderSourceFileStr, nextNewLineIndex+1));
}

void CreateVertBuffer(VertBuffer_t* bufferOut, bool dynamic, u32 vertexSize, u32 numVertices, const void* verticesPntr)
{
	bufferOut->glId = glGenBuffer();
	bufferOut->vertexSize = vertexSize;
	bufferOut->numVertices = numVertices;
	glBindBuffer(GL_ARRAY_BUFFER, bufferOut->glId);
	glBufferData(GL_ARRAY_BUFFER, numVertices * vertexSize, verticesPntr, (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
}

void CreateVertArrayObj(VertArrayObj_t* vaoOut, bool is3d, u8 attributes)
{
	vaoOut->glId = glGenVertexArray();
	vaoOut->is3d = is3d;
	vaoOut->attributes = attributes;
}

void CreateTexture(Texture_t* textureOut, v2i size, const u32* pixels, bool pixelated, bool wrapping)
{
	NotNull2(textureOut, pixels);
	
	ClearPointer(textureOut);
	textureOut->glId = glGenTexture();
	textureOut->size = size;
	glBindTexture(GL_TEXTURE_2D, textureOut->glId);
	glTexImage2D(
		GL_TEXTURE_2D,    //bound texture type
		0,                //image level
		GL_RGBA,          //internal format
		size.x,           //image width
		size.y,           //image height
		0,                //border
		GL_RGBA,          //format
		GL_UNSIGNED_BYTE, //type
		pixels            //data
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (pixelated ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (pixelated ? GL_NEAREST : GL_LINEAR));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     (wrapping ? GL_REPEAT : GL_CLAMP_TO_EDGE));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     (wrapping ? GL_REPEAT : GL_CLAMP_TO_EDGE));
	glGenerateMipmap(GL_TEXTURE_2D);
	textureOut->isValid = true;
}

bool CreateTextureFromPngFile(Texture_t* textureOut, u32 fileSize, const void* filePntr, bool pixelated, bool wrapping)
{
	int stbNumChannels;
	int stbWidth, stbHeight;
	u8* stbPixelData = stbi_load_from_memory(
		(const u8*)filePntr, (int)fileSize,
		&stbWidth, &stbHeight,
		&stbNumChannels,
		sizeof(u32)
	);
	UNUSED(stbNumChannels);
	if (stbPixelData == nullptr)
	{
		PrintLine_E("Failed to parse image: %s", stbi_failure_reason());
		//TODO: DestroyTexture?
		return false;
	}
	Assert(stbWidth > 0 && stbHeight > 0);
	
	v2i textureSize = NewVec2i(stbWidth, stbHeight);
	CreateTexture(textureOut, textureSize, (const u32*)stbPixelData, pixelated, wrapping);
	
	stbi_image_free(stbPixelData);
	
	return true;
}

// +--------------------------------------------------------------+
// |                          StartLoad                           |
// +--------------------------------------------------------------+
bool StartLoadShader(Shader_t* targetShader, MyStr_t filePath)
{
	ShaderLoadContext_t* shaderLoadContext = NewShaderLoadContext(targetShader);
	if (shaderLoadContext == nullptr) { return false; }
	
	FileSystemRequestAsync(filePath, FileLoadingManagerShaderFileReadyCallback, shaderLoadContext);
	return true;
}
bool StartLoadTexture(Texture_t* targetTexture, MyStr_t filePath, bool pixelated, bool wrapping)
{
	TextureLoadContext_t* textureLoadContext = NewTextureLoadContext(targetTexture, pixelated, wrapping);
	if (textureLoadContext == nullptr) { return false; }
	
	FileSystemRequestAsync(filePath, FileLoadingManagerTextureFileReadyCallback, textureLoadContext);
	return true;
}

// +--------------------------------------------------------------+
// |                             Bind                             |
// +--------------------------------------------------------------+
void BindShader(Shader_t* shader)
{
	glUseProgram(shader->glId);
}
void BindVertBuffer(VertBuffer_t* buffer)
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer->glId);
}
void BindVertArrayObj(VertArrayObj_t* vao)
{
	glBindVertexArray(vao->glId);
	
	u32 stride = 0;
	u32 attributeIndex = 0;
	for (u8 aIndex = 0; aIndex < VertexAttribute_NumAttributes; aIndex++)
	{
		VertexAttribute_t attribute = (VertexAttribute_t)(1 << aIndex);
		if (IsFlagSet(vao->attributes, attribute))
		{
			glEnableVertexAttribArray(attributeIndex);
			if      (attribute == VertexAttribute_Position && !vao->is3d) { stride += sizeof(v2); }
			else if (attribute == VertexAttribute_Position &&  vao->is3d) { stride += sizeof(v3); }
			else if (attribute == VertexAttribute_Color)                  { stride += sizeof(v4); }
			else if (attribute == VertexAttribute_TexCoord)               { stride += sizeof(v2); }
			else if (attribute == VertexAttribute_Normal)                 { stride += sizeof(v3); }
			attributeIndex++;
		}
	}
	
	attributeIndex = 0;
	u32 attributeOffset = 0;
	for (u8 aIndex = 0; aIndex < VertexAttribute_NumAttributes; aIndex++)
	{
		VertexAttribute_t attribute = (VertexAttribute_t)(1 << aIndex);
		if (IsFlagSet(vao->attributes, attribute))
		{
			if (attribute == VertexAttribute_Position && !vao->is3d)
			{
				glVertexAttribPointer(attributeIndex, 2, GL_FLOAT, false, stride, (const void*)attributeOffset);
				attributeOffset += sizeof(v2);
			}
			else if (attribute == VertexAttribute_Position && vao->is3d)
			{
				glVertexAttribPointer(attributeIndex, 3, GL_FLOAT, false, stride, (const void*)attributeOffset);
				attributeOffset += sizeof(v3);
			}
			else if (attribute == VertexAttribute_Color)
			{
				glVertexAttribPointer(attributeIndex, 4, GL_FLOAT, false, stride, (const void*)attributeOffset);
				attributeOffset += sizeof(v4);
			}
			else if (attribute == VertexAttribute_TexCoord)
			{
				glVertexAttribPointer(attributeIndex, 2, GL_FLOAT, false, stride, (const void*)attributeOffset);
				attributeOffset += sizeof(v2);
			}
			else if (attribute == VertexAttribute_Normal)
			{
				glVertexAttribPointer(attributeIndex, 3, GL_FLOAT, false, stride, (const void*)attributeOffset);
				attributeOffset += sizeof(v3);
			}
			attributeIndex++;
		}
	}
}
void BindTexture1(Shader_t* boundShader, Texture_t* texture)
{
	NotNull(boundShader);
	NotNull(texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->glId);
	glUniform1i(boundShader->uniformLocations[ShaderUniform_Texture1], 0);
	glUniform2f(boundShader->uniformLocations[ShaderUniform_Texture1Size], texture->size.x, texture->size.y);
}
