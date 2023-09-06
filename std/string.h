/*
File:   string.h
Author: Taylor Robbins
Date:   11\23\2022
*/

#ifndef _STRING_H
#define _STRING_H

#ifndef NULL
#define NULL nullptr
#endif

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

//TODO: Optimize this implementation!
int memcmp(const void* left, const void* right, size_t numBytes)
{
	const unsigned char* leftPntr = (const unsigned char*)left;
	const unsigned char* rightPntr = (const unsigned char*)right;
	size_t bIndex;
	for (bIndex = 0; bIndex < numBytes && *leftPntr == *rightPntr; bIndex++)
	{
		leftPntr++;
		rightPntr++;
	}
	return ((bIndex < numBytes) ? (int)(*leftPntr) - (int)(*rightPntr) : 0);
}

//TODO: Optimize this implementation
int strcmp(const char* left, const char* right)
{
	size_t charIndex;
	for (charIndex = 0; left[charIndex] != '\0' && right[charIndex] != '\0'; charIndex++)
	{
		if (left[charIndex] != right[charIndex])
		{
			return ((left[charIndex] > right[charIndex]) ? 1 : -1);
		}
	}
	if (left[charIndex] == '\0' && right[charIndex] == '\0') { return 0; }
	return ((left[charIndex] == '\0') ? -1 : 1);
}

//TODO: Optimize this implementation
int strncmp(const char* left, const char* right, size_t numChars)
{
	size_t charIndex;
	for (charIndex = 0; charIndex < numChars && left[charIndex] != '\0' && right[charIndex] != '\0'; charIndex++)
	{
		if (left[charIndex] != right[charIndex])
		{
			return ((left[charIndex] > right[charIndex]) ? 1 : -1);
		}
	}
	if (charIndex >= numChars) { return 0; }
	if (left[charIndex] == '\0' && right[charIndex] == '\0') { return 0; }
	return ((left[charIndex] == '\0') ? -1 : 1);
}

#endif //  _STRING_H
