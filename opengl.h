/*
File:   opengl.h
Author: Taylor Robbins
Date:   11\23\2022
*/

#ifndef _OPENGL_H
#define _OPENGL_H

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
typedef i8     GLbyte;
typedef u8     GLubyte;
typedef i16    GLshort;
typedef u16    GLushort;
typedef i32    GLint;
typedef u32    GLuint;
typedef i64    GLint64;
typedef u64    GLuint64;

typedef r32    GLfloat;
typedef r64    GLdouble;
typedef r32    GLclampf;
typedef r64    GLclampd;

typedef bool   GLboolean;
typedef i32    GLenum;
typedef u32    GLsizei;
typedef char   GLchar;
typedef u32    GLbitfield;
typedef i32    GLintptr;
typedef u32    GLsizeiptr;

// TODO: Add these typedefs?
// GLfixed 	32 	Signed, 2's complement 16.16 integer 	GL_FIXED
// GLsync 	ptrbits​1 	Sync Object handle 	
// GLhalf 	16 	An IEEE-754 floating-point value 	GL_HALF_FLOAT

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define GL_ONE                       1
#define GL_TRIANGLES                 0x0004
#define GL_DEPTH_BUFFER_BIT          0x00000100
#define GL_STENCIL_BUFFER_BIT        0x00000400
#define GL_COLOR_BUFFER_BIT          0x00004000
#define GL_CULL_FACE                 0x0B44
#define GL_BLEND                     0x0BE2
#define GL_VIEWPORT                  0x0BA2
#define GL_TEXTURE_2D                0x0DE1
#define GL_BYTE                      0x1400
#define GL_UNSIGNED_BYTE             0x1401
#define GL_SHORT                     0x1402
#define GL_UNSIGNED_SHORT            0x1403
#define GL_INT                       0x1404
#define GL_UNSIGNED_INT              0x1405
#define GL_FLOAT                     0x1406
#define GL_UNSIGNED_BYTE             0x1401
#define GL_RGBA                      0x1908
#define GL_NEAREST                   0x2600
#define GL_LINEAR                    0x2601
#define GL_NEAREST_MIPMAP_NEAREST    0x2700
#define GL_LINEAR_MIPMAP_NEAREST     0x2701
#define GL_NEAREST_MIPMAP_LINEAR     0x2702
#define GL_LINEAR_MIPMAP_LINEAR      0x2703
#define GL_TEXTURE_MAG_FILTER        0x2800
#define GL_TEXTURE_MIN_FILTER        0x2801
#define GL_TEXTURE_WRAP_S            0x2802
#define GL_TEXTURE_WRAP_T            0x2803
#define GL_REPEAT                    0x2901
#define GL_SRC_ALPHA                 0x0302
#define GL_ONE_MINUS_SRC_ALPHA       0x0303
#define GL_CLAMP_TO_EDGE             0x812F
#define GL_TEXTURE0                  0x84C0
#define GL_ARRAY_BUFFER              0x8892
#define GL_STATIC_DRAW               0x88E4
#define GL_DYNAMIC_DRAW              0x88E8
#define GL_FRAGMENT_SHADER           0x8B30
#define GL_VERTEX_SHADER             0x8B31
#define GL_COMPILE_STATUS            0x8B81
#define GL_LINK_STATUS               0x8B82
#define GL_SHADER_COMPILER           0x8DFA
#endif //  _OPENGL_H
