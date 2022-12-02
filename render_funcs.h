/*
File:   render_funcs.h
Author: Taylor Robbins
Date:   11\27\2022
*/

#ifndef _RENDER_FUNCS_H
#define _RENDER_FUNCS_H

#include "graphics_base.h"
#include "matrices.h"
#include "vectors.h"
#include "color.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct RenderContext_t
{
	VertBuffer_t squareBuffer;
	VertArrayObj_t basicVao;
	
	Shader_t* boundShader;
	VertBuffer_t* boundBuffer;
	VertArrayObj_t* boundVao;
	
	mat4 worldMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	
	v4 color;
};

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
extern RenderContext_t* rc;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void InitializeRenderContext();
void RcBindShader(Shader_t* shader);
void RcBindVertBuffer(VertBuffer_t* buffer);
void RcBindVertArrayObj(VertArrayObj_t* vao);
void RcSetColor(Color_t color);
void RcSetWorldMatrix(const mat4& matrix);
void RcSetViewMatrix(const mat4& matrix);
void RcSetProjectionMatrix(const mat4& matrix);
void RcDrawRectangle(v2 position, v2 size, Color_t color);


#endif //  _RENDER_FUNCS_H
