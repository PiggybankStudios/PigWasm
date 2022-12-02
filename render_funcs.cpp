/*
File:   render_funcs.cpp
Author: Taylor Robbins
Date:   11\27\2022
Description: 
	** Holds some helper functions that make rendering various primitives easier
*/

#include "common.h"
#include "render_funcs.h"

#include "debug.h"
#include "memory.h"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
RenderContext_t* rc = nullptr;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void InitializeRenderContext()
{
	rc = AllocStruct(mainHeap, RenderContext_t);
	NotNull(rc);
	ClearPointer(rc);
	
	Vertex2D_t squareVertices[6];
	squareVertices[0].position = NewVec2(0, 0); squareVertices[0].color = NewVec4(1, 1, 1, 1); squareVertices[0].texCoord = NewVec2(0, 0);
	squareVertices[1].position = NewVec2(1, 0); squareVertices[1].color = NewVec4(1, 1, 1, 1); squareVertices[1].texCoord = NewVec2(1, 0);
	squareVertices[2].position = NewVec2(0, 1); squareVertices[2].color = NewVec4(1, 1, 1, 1); squareVertices[2].texCoord = NewVec2(0, 1);
	squareVertices[3].position = NewVec2(1, 0); squareVertices[3].color = NewVec4(1, 1, 1, 1); squareVertices[3].texCoord = NewVec2(1, 0);
	squareVertices[4].position = NewVec2(1, 1); squareVertices[4].color = NewVec4(1, 1, 1, 1); squareVertices[4].texCoord = NewVec2(1, 1);
	squareVertices[5].position = NewVec2(0, 1); squareVertices[5].color = NewVec4(1, 1, 1, 1); squareVertices[5].texCoord = NewVec2(0, 1);
	CreateVertBuffer(&rc->squareBuffer, false, sizeof(Vertex2D_t), ArrayCount(squareVertices), &squareVertices[0]);
	
	CreateVertArrayObj(&rc->basicVao, false, VertexAttribute_Position|VertexAttribute_Color|VertexAttribute_TexCoord);
}

void RcBindShader(Shader_t* shader)
{
	BindShader(shader);
	rc->boundShader = shader;
}
void RcBindVertBuffer(VertBuffer_t* buffer)
{
	BindVertBuffer(buffer);
	rc->boundBuffer = buffer;
}
void RcBindVertArrayObj(VertArrayObj_t* vao)
{
	BindVertArrayObj(vao);
	rc->boundVao = vao;
}

void RcSetColor(Color_t color)
{
	rc->color = ToVec4(color);
	glUniform4f(rc->boundShader->uniformLocations[ShaderUniform_Color1], rc->color.x, rc->color.y, rc->color.z, rc->color.w);
}

void RcSetWorldMatrix(const mat4& matrix)
{
	glUniformMatrix4fv(
		rc->boundShader->uniformLocations[ShaderUniform_WorldMatrix],
		1,
		false,
		&matrix.values[0][0]
	);
	rc->worldMatrix = matrix;
}
void RcSetViewMatrix(const mat4& matrix)
{
	glUniformMatrix4fv(
		rc->boundShader->uniformLocations[ShaderUniform_ViewMatrix],
		1,
		false,
		&matrix.values[0][0]
	);
	rc->viewMatrix = matrix;
}
void RcSetProjectionMatrix(const mat4& matrix)
{
	glUniformMatrix4fv(
		rc->boundShader->uniformLocations[ShaderUniform_ProjectionMatrix],
		1,
		false,
		&matrix.values[0][0]
	);
	rc->projectionMatrix = matrix;
}

void RcDrawRectangle(v2 position, v2 size, Color_t color)
{
	Assert(rc->boundShader != nullptr);
	mat4 worldMatrix = Mat4Multiply(Mat4Translate2(position.x, position.y), Mat4Scale2(size.x, size.y));
	RcSetWorldMatrix(worldMatrix);
	RcSetColor(color);
	RcBindVertBuffer(&rc->squareBuffer);
	RcBindVertArrayObj(&rc->basicVao);
	glDrawArrays(GL_TRIANGLES, 0, rc->squareBuffer.numVertices);
}