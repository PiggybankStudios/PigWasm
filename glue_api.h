/*
File:   glue_api.h
Author: Taylor Robbins
Date:   11\22\2022
This is #included by common.h
*/

#ifndef _GLUE_API_H
#define _GLUE_API_H

#define GLUE_FUNC extern "C"

// +--------------------------------------------------------------+
// |                       Custom Functions                       |
// +--------------------------------------------------------------+
GLUE_FUNC void RequestMoreMemoryPages(u32 numPages);
GLUE_FUNC void PrintoutStack();
GLUE_FUNC void DebugOutput(int level, const char* message);
GLUE_FUNC void GetCanvasSize(r32* widthOut, r32* heightOut);
GLUE_FUNC void GetMousePosition(r32* xPosOut, r32* yPosOut);
GLUE_FUNC void TestFunction();

// +--------------------------------------------------------------+
// |                       OpenGL Functions                       |
// +--------------------------------------------------------------+
GLUE_FUNC GLint  glGetError();
GLUE_FUNC void   glGetBooleanv(GLenum pname, GLboolean* params);
GLUE_FUNC void   glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
GLUE_FUNC void   glGetProgramiv(GLuint program, GLenum pname, GLint* params);
GLUE_FUNC void   glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
GLUE_FUNC void   glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
GLUE_FUNC GLint  glGetAttribLocation(GLuint program, const GLchar* name);
GLUE_FUNC GLint  glGetUniformLocation(GLuint program, const GLchar* name);
GLUE_FUNC void   glGetTextureSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void* pixels);
GLUE_FUNC GLenum glCheckFramebufferStatus(GLenum target);
GLUE_FUNC void   glEnable(GLenum cap);
GLUE_FUNC void   glDisable(GLenum cap);
GLUE_FUNC void   glBlendFunc(GLenum sfactor, GLenum dfactor);
GLUE_FUNC void   glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
GLUE_FUNC void   glDepthFunc(GLenum func);
GLUE_FUNC void   glAlphaFunc(GLenum func, GLclampf ref);
GLUE_FUNC void   glFrontFace(GLenum mode);
GLUE_FUNC void   glLineWidth(GLfloat width);
GLUE_FUNC GLuint glGenFramebuffer(); //changed
GLUE_FUNC GLuint glGenVertexArray(); //changed
GLUE_FUNC GLuint glGenTexture(); //changed
GLUE_FUNC GLuint glGenBuffer(); //changed
GLUE_FUNC GLuint glCreateShader(GLenum shaderType);
GLUE_FUNC GLuint glCreateProgram();
GLUE_FUNC void   glDeleteFramebuffer(GLuint framebuffer); //changed
GLUE_FUNC void   glDeleteTexture(GLuint texture); //changed
GLUE_FUNC void   glDeleteShader(GLuint shader);
GLUE_FUNC void   glDeleteProgram(GLuint program);
GLUE_FUNC void   glDeleteBuffer(GLuint buffer); //changed
GLUE_FUNC void   glBindFramebuffer(GLenum target, GLuint framebuffer);
GLUE_FUNC void   glBindVertexArray(GLuint array);
GLUE_FUNC void   glBindTexture(GLenum target, GLuint texture);
GLUE_FUNC void   glBindBuffer(GLenum target, GLuint buffer);
GLUE_FUNC void   glUseProgram(GLuint program);
GLUE_FUNC void   glTexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
GLUE_FUNC void   glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data);
GLUE_FUNC void   glTexParameteri(GLenum target, GLenum pname, GLint param);
GLUE_FUNC void   glTexParameteriv(GLenum target, GLenum pname, const GLint* params);
GLUE_FUNC void   glEnableVertexAttribArray(GLuint index);
GLUE_FUNC void   glActiveTexture(GLenum texture);
GLUE_FUNC void   glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
GLUE_FUNC void   glShaderSource(GLuint shader, const GLchar* string); //changed
GLUE_FUNC void   glCompileShader(GLuint shader);
GLUE_FUNC void   glAttachShader(GLuint program, GLuint shader);
GLUE_FUNC void   glLinkProgram(GLuint program);
GLUE_FUNC void   glGenerateMipmap(GLenum target);
GLUE_FUNC void   glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
GLUE_FUNC void   glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
GLUE_FUNC void   glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
GLUE_FUNC void   glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GLUE_FUNC void   glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLUE_FUNC void   glUniform1i(GLint location, GLint v0);
GLUE_FUNC void   glUniform1f(GLint location, GLfloat v0);
GLUE_FUNC void   glUniform2f(GLint location, GLfloat v0, GLfloat v1);
GLUE_FUNC void   glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GLUE_FUNC void   glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLUE_FUNC void   glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GLUE_FUNC void   glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
GLUE_FUNC void   glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
GLUE_FUNC void   glClearDepth(GLdouble depth);
GLUE_FUNC void   glClearStencil(GLint s);
GLUE_FUNC void   glClear(GLbitfield mask);
GLUE_FUNC void   glDrawArrays(GLenum mode, GLint first, GLsizei count);
GLUE_FUNC void   glDrawBuffers(GLsizei n, const GLenum* bufs);

#endif //  _GLUE_API_H
