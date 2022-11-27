/*
File:   string.h
Author: Taylor Robbins
Date:   11\23\2022
*/

#ifndef _STRING_H
#define _STRING_H

//TODO: Optimize this implementation!
inline size_t strlen(const char* str)
{
	size_t result = 0;
	while (str[result] != '\0') { result++; }
	return result;
}

//TODO: Optimize this implementation!
void* memcpy(void* __restrict dest, const void* __restrict src, size_t count)
{
	for (size_t bIndex = 0; bIndex < count; bIndex++)
	{
		((uint8_t*)dest)[bIndex] = ((uint8_t*)src)[bIndex];
	}
	return dest;
}

//TODO: Optimize this implementation!
void* memset(void* dest, int value, size_t num)
{
	for (size_t bIndex = 0; bIndex < num; bIndex++)
	{
		((uint8_t*)dest)[bIndex] = (uint8_t)value;
	}
	return dest;
}

#endif //  _STRING_H
