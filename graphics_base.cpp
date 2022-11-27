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

static GLchar parseLogBuffer[1024];

// +--------------------------------------------------------------+
// |                            Create                            |
// +--------------------------------------------------------------+
void CreateShader(Shader_t* shaderOut, const char* vertSource, const char* fragSource)
{
	GLsizei parseLogLength = 0;
	GLint linkStatus = 0;
	
	shaderOut->vertId = glCreateShader(GL_VERTEX_SHADER);
	shaderOut->fragId = glCreateShader(GL_FRAGMENT_SHADER);
	shaderOut->glId = glCreateProgram();
	// PrintLine_D("vert %d frag %d program %d", shaderOut->vertId, shaderOut->fragId, shaderOut->glId);
	
	glShaderSource(shaderOut->vertId, vertSource);
	glCompileShader(shaderOut->vertId);
	glGetShaderInfoLog(shaderOut->vertId, ArrayCount(parseLogBuffer), &parseLogLength, &parseLogBuffer[0]);
	if (parseLogLength > 0)
	{
		PrintLine_W("Vertex shader compilation had %d char log:\n%s", parseLogLength, &parseLogBuffer[0]);
	}
	
	glShaderSource(shaderOut->fragId, fragSource);
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
