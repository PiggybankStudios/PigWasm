/*
File:   stb_image_wrapper.cpp
Author: Taylor Robbins
Date:   12\01\2022
Description: 
	** Acts as a wrapper around the implementation of stb_image.h so our BuildCheck.py
	** can try compiling this cpp as a standalone unit
*/

#include "common.h"

#include "memory.h"

void* StbImageMalloc(size_t numBytes)
{
	return AllocMem(mainHeap, (u64)numBytes);
}
void StbImageFree(void* pntr)
{
	FreeMem(mainHeap, pntr);
}
void* StbImageRealloc(void* pntr, size_t newSize)
{
	return ReallocMem(mainHeap, pntr, (u64)newSize);
}

#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(size)  StbImageMalloc(size)
#define STBI_FREE(pntr)    StbImageFree(pntr)
#define STBI_REALLOC(pntr, newSize) StbImageRealloc((pntr), (newSize))
#define STBI_ASSERT(condition) Assert(condition)
#include "stb/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
