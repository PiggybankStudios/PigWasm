/*
File:   graphics_base.h
Author: Taylor Robbins
Date:   11\23\2022
*/

#include "vectors.h"

#ifndef _GRAPHICS_BASE_H
#define _GRAPHICS_BASE_H

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define FRAGMENT_VERTEX_DIVIDER_STR "FRAGMENT_SHADER"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct Vertex2D_t
{
	v2 position;
	v4 color;
	v2 texCoord;
};
struct Vertex3D_t
{
	v3 position;
	v4 color;
	v2 texCoord;
	v3 normal;
};

enum VertexAttribute_t
{
	VertexAttribute_None     = 0x00,
	VertexAttribute_Position = 0x01,
	VertexAttribute_Color    = 0x02,
	VertexAttribute_TexCoord = 0x04,
	VertexAttribute_Normal   = 0x08,
	VertexAttribute_All      = 0x0F,
	VertexAttribute_NumAttributes = 4,
};
inline const char* GetVertexAttributeGlslName(VertexAttribute_t attribute)
{
	switch (attribute)
	{
		case VertexAttribute_Position: return "inPosition";
		case VertexAttribute_Color:    return "inColor";
		case VertexAttribute_TexCoord: return "inTexCoord";
		case VertexAttribute_Normal:   return "inNormal";
		default: return "Unknown";
	}
}

enum ShaderUniform_t
{
	ShaderUniform_WorldMatrix = 0,
	ShaderUniform_ViewMatrix,
	ShaderUniform_ProjectionMatrix,
	ShaderUniform_Texture1,
	ShaderUniform_Texture1Size,
	ShaderUniform_Texture2,
	ShaderUniform_Texture2Size,
	ShaderUniform_SourceRec1,
	ShaderUniform_SourceRec2,
	ShaderUniform_Color1,
	ShaderUniform_Color2,
	ShaderUniform_NumUniforms,
};
inline const char* GetShaderUniformGlslName(ShaderUniform_t uniform)
{
	switch (uniform)
	{
		case ShaderUniform_WorldMatrix:      return "WorldMatrix";
		case ShaderUniform_ViewMatrix:       return "ViewMatrix";
		case ShaderUniform_ProjectionMatrix: return "ProjectionMatrix";
		case ShaderUniform_Texture1:         return "Texture1";
		case ShaderUniform_Texture1Size:     return "Texture1Size";
		case ShaderUniform_Texture2:         return "Texture2";
		case ShaderUniform_Texture2Size:     return "Texture2Size";
		case ShaderUniform_SourceRec1:       return "SourceRec1";
		case ShaderUniform_SourceRec2:       return "SourceRec2";
		case ShaderUniform_Color1:           return "Color1";
		case ShaderUniform_Color2:           return "Color2";
		default: return "Unknown";
	}
}

struct Shader_t
{
	bool isValid;
	GLuint vertId;
	GLuint fragId;
	GLuint glId;
	
	u8 attributes;
	GLint attributeLocations[VertexAttribute_NumAttributes];
	GLint uniformLocations[ShaderUniform_NumUniforms];
};

struct VertBuffer_t
{
	GLuint glId;
	u32 vertexSize;
	u32 numVertices;
};

struct VertArrayObj_t
{
	GLuint glId;
	bool is3d;
	u8 attributes;
};

struct Texture_t
{
	bool isValid;
	GLuint glId;
	v2i size;
};

struct ShaderLoadContext_t
{
	bool filled;
	Shader_t* targetShader;
};

struct TextureLoadContext_t
{
	bool filled;
	Texture_t* targetTexture;
	bool pixelated;
	bool wrapping;
};

#define MAX_NUM_CONCURRENT_SHADER_LOADS 16
#define MAX_NUM_CONCURRENT_TEXTURE_LOADS 16

struct FileLoadingManager_t
{
	ShaderLoadContext_t shaderLoadContexts[MAX_NUM_CONCURRENT_SHADER_LOADS];
	TextureLoadContext_t textureLoadContexts[MAX_NUM_CONCURRENT_TEXTURE_LOADS];
};

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
extern FileLoadingManager_t* fileLoadingManager;

// +--------------------------------------------------------------+
// |                        Init and Free                         |
// +--------------------------------------------------------------+
void InitFileLoadingManager(FileLoadingManager_t* manager);

// +--------------------------------------------------------------+
// |                            Create                            |
// +--------------------------------------------------------------+
void CreateShader(Shader_t* shaderOut, MyStr_t vertSource, MyStr_t fragSource);
void CreateShaderFromSingleStr(Shader_t* shaderOut, MyStr_t shaderSourceFileStr);
void CreateVertBuffer(VertBuffer_t* bufferOut, bool dynamic, u32 vertexSize, u32 numVertices, const void* verticesPntr);
void CreateVertArrayObj(VertArrayObj_t* vaoOut, bool is3d, u8 attributes);
void CreateTexture(Texture_t* textureOut, v2i size, const u32* pixels, bool pixelated, bool wrapping);
bool CreateTextureFromPngFile(Texture_t* textureOut, u32 fileSize, const void* filePntr, bool pixelated, bool wrapping);

// +--------------------------------------------------------------+
// |                          StartLoad                           |
// +--------------------------------------------------------------+
//TODO: Add some sort of async failed callback or something for these
bool StartLoadShader(Shader_t* targetShader, MyStr_t filePath);
bool StartLoadTexture(Texture_t* targetTexture, MyStr_t filePath, bool pixelated, bool wrapping);

// +--------------------------------------------------------------+
// |                             Bind                             |
// +--------------------------------------------------------------+
void BindShader(Shader_t* shader);
void BindVertBuffer(VertBuffer_t* buffer);
void BindVertArrayObj(VertArrayObj_t* vao);
void BindTexture1(Texture_t* texture);

#endif //  _GRAPHICS_BASE_H
