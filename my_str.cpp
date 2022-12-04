/*
File:   my_str.cpp
Author: Taylor Robbins
Date:   12\01\2022
Description: 
	** MyStr_t is a simple structure that holds a size and pointer
	** It's mostly used for "strings" of characters that were allocated together
	** or for views into subsections of other strings that were allocated together
	** It is not const correct, nor does it save how the memory was allocated so
	** it's up to the usage code to keep track of these aspects 
*/

#include "common.h"
#include "my_str.h"

#include "memory.h"

MyStr_t NewStr(const char* nullTermStr)
{
	MyStr_t result;
	result.length = (u32)strlen(nullTermStr);
	result.chars = (char*)nullTermStr;
	return result;
}

MyStr_t PrintInArenaStr(MemArena_t* arena, const char* formatString, ...)
{
	NotNull(arena);
	NotNull(formatString);
	
	char* result = nullptr;
	va_list args;
	
	va_start(args, formatString);
	int length = vsnprintf(result, 0, formatString, args); //Measure first
	Assert(length >= 0);
	va_end(args);
	
	result = AllocArray(arena, char, length+1); //Allocate
	if (result == nullptr) { return MyStr_Empty; }
	
	va_start(args, formatString);
	vsnprintf(result, (size_t)(length+1), formatString, args); //Real printf
	va_end(args);
	
	result[length] = '\0';
	
	return NewStr((u32)length, result);
}

bool StrEquals(MyStr_t left, MyStr_t right)
{
	NotNullStr(&left);
	NotNullStr(&right);
	if (left.pntr == nullptr && right.pntr == nullptr) { return true; }
	if (left.length != right.length) { return false; }
	if (left.length == 0 && right.length == 0) { return true; }
	if (left.pntr == nullptr || right.pntr == nullptr) { AssertMsg(false, "this case is only for malformed MyStr_t structs (size != 0 but pntr is nullptr)"); return false; }
	return (memcmp(left.pntr, right.pntr, left.length) == 0);
}
bool StrEquals(MyStr_t left, const char* rightNt)
{
	NotNull(rightNt);
	u32 rightLength = strlen(rightNt);
	return StrEquals(left, NewStr(rightLength, rightNt));
}

i32 StrCompareIgnoreCase(MyStr_t left, MyStr_t right, u32 compareLength, u32 leftOffset, u32 rightOffset)
{
	NotNullStr(&left);
	NotNullStr(&right);
	Assert(leftOffset + compareLength <= left.length);
	Assert(rightOffset + compareLength <= right.length);
	for (u32 cIndex = 0; cIndex < compareLength; cIndex++)
	{
		if (leftOffset + cIndex >= left.length && rightOffset + cIndex >= right.length)
		{
			return 0; //they were equal all the way up until they both ended early
		}
		else if (leftOffset + cIndex >= left.length)
		{
			return -1; //left ended earlier than right
		}
		else if (rightOffset + cIndex >= right.length)
		{
			return 1; //right ended earlier than left
		}
		else
		{
			char char1 = GetLowercaseAnsiiChar(left.chars[leftOffset + cIndex]);
			char char2 = GetLowercaseAnsiiChar(right.chars[rightOffset + cIndex]);
			if (char1 > char2)
			{
				return 1; //left comes after right alphabetically (left > right)
			}
			else if (char2 > char1)
			{
				return -1; //right comes after left alphabetically (left < right)
			}
			else { continue; }
		}
	}
	return 0;
}
i32 StrCompareIgnoreCase(MyStr_t left, MyStr_t right)
{
	u32 minLength = MIN(left.length, right.length);
	i32 compareResult = StrCompareIgnoreCase(left, right, minLength);
	if (compareResult != 0) { return compareResult; }
	if (left.length > right.length) { return 1; }
	if (left.length < right.length) { return -1; }
	return 0;
}
i32 StrCompareIgnoreCase(MyStr_t left, const char* rightNt)
{
	u32 rightLength = (u32)strlen(rightNt);
	return StrCompareIgnoreCase(left, NewStr(rightLength, rightNt));
}

void SplitFilePath(MyStr_t fullPath, MyStr_t* directoryOut, MyStr_t* fileNameOut, MyStr_t* extensionOut)
{
	NotNullStr(&fullPath);
	if (fullPath.length == 0)
	{
		if (directoryOut != nullptr) { *directoryOut = MyStr_Empty; }
		if (fileNameOut != nullptr) { *fileNameOut = MyStr_Empty; }
		if (extensionOut != nullptr) { *extensionOut = MyStr_Empty; }
		return;
	}
	
	bool foundSlash = false;
	bool foundPeriod = false;
	u64 lastSlashIndex = 0; //index after char
	u64 lastPeriodIndex = fullPath.length; //index before char
	for (u64 cIndex = 0; cIndex < fullPath.length; cIndex++)
	{
		if (fullPath.chars[cIndex] == '\\' || fullPath.chars[cIndex] == '/')
		{
			foundSlash = true;
			lastSlashIndex = cIndex+1;
		}
		if (fullPath.chars[cIndex] == '.')
		{
			foundPeriod = true;
			lastPeriodIndex = cIndex;
		}
	}
	if (foundPeriod && lastPeriodIndex < lastSlashIndex) //periods in the directory portion are relative directives like ".\..\" 
	{
		foundPeriod = false;
		lastPeriodIndex = fullPath.length;
	}
	Assert(lastPeriodIndex >= lastSlashIndex);
	
	if (directoryOut != nullptr)
	{
		*directoryOut = NewStr(lastSlashIndex, &fullPath.chars[0]);
	}
	if (fileNameOut != nullptr)
	{
		if (extensionOut != nullptr)
		{
			*fileNameOut = NewStr(lastPeriodIndex - lastSlashIndex, &fullPath.chars[lastSlashIndex]);
			*extensionOut = NewStr(fullPath.length - lastPeriodIndex, &fullPath.chars[lastPeriodIndex]);
		}
		else
		{
			*fileNameOut = NewStr(fullPath.length - lastSlashIndex, &fullPath.chars[lastSlashIndex]);
		}
	}
}

//TODO: This should return true if target and substring are equal!
bool FindSubstring(MyStr_t target, MyStr_t substring, u32* indexOut, u32 startIndex)
{
	NotNullStr(&target);
	NotNullStr(&substring);
	if (substring.length > target.length) { return false; }
	
	for (u32 cIndex = startIndex; cIndex + substring.length <= target.length; )
	{
		bool allMatched = true;
		u32 cSubIndex = 0;
		for (u32 subIndex = 0; subIndex < substring.length; )
		{
			u32 targetCodepoint = CharToU32(target.chars[cIndex + cSubIndex]);
			u32 subCodepoint = CharToU32(substring.chars[subIndex]);
			
			if (targetCodepoint != subCodepoint)
			{
				allMatched = false;
				break;
			}
			
			subIndex++;
			cSubIndex++;
		}
		if (allMatched)
		{
			if (indexOut != nullptr) { *indexOut = cIndex; }
			return true;
		}
		else
		{
			cIndex++;
		}
	}
	
	return false;
}
