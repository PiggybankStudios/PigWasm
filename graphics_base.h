/*
File:   graphics_base.h
Author: Taylor Robbins
Date:   11\23\2022
*/

#include "vectors.h"

#ifndef _GRAPHICS_BASE_H
#define _GRAPHICS_BASE_H

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

struct Shader_t
{
	GLuint vertId;
	GLuint fragId;
	GLuint glId;
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

// +--------------------------------------------------------------+
// |                            Create                            |
// +--------------------------------------------------------------+
void CreateShader(Shader_t* shaderOut, const char* vertSource, const char* fragSource);
void CreateVertBuffer(VertBuffer_t* bufferOut, bool dynamic, u32 vertexSize, u32 numVertices, const void* verticesPntr);
void CreateVertArrayObj(VertArrayObj_t* vaoOut, bool is3d, u8 attributes);

// +--------------------------------------------------------------+
// |                             Bind                             |
// +--------------------------------------------------------------+
void BindShader(Shader_t* shader);
void BindVertBuffer(VertBuffer_t* buffer);

#endif //  _GRAPHICS_BASE_H
