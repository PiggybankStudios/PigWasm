/*
File:   my_str.h
Author: Taylor Robbins
Date:   12\01\2022
This is #included by common.h
*/

#ifndef _MY_STR_H
#define _MY_STR_H

#include "memory_arena.h"

// +--------------------------------------------------------------+
// |                          Structure                           |
// +--------------------------------------------------------------+
struct MyStr_t
{
	union
	{
		u32 length;
		u32 count;
		u32 size;
		u32 numChars;
	};
	union
	{
		void* pntr;
		u8* bytes;
		char* chars;
	};
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define MyStr_Empty NewStr((u32)0, (char*)nullptr)

#define NotNullStr(strPntr)      Assert (!IsNullStr(strPntr))
#define NotNullStr_(strPntr)     Assert_(!IsNullStr(strPntr))
#define NotEmptyStr(strPntr)     Assert ((strPntr) != nullptr && !IsEmptyStr(strPntr))
#define NotEmptyStr_(strPntr)    Assert_((strPntr) != nullptr && !IsEmptyStr(strPntr))
#define AssertNullTerm(strPntr)  Assert ((strPntr) != nullptr && IsStrNullTerminated(strPntr))
#define AssertNullTerm_(strPntr) Assert_((strPntr) != nullptr && IsStrNullTerminated(strPntr))

#define AllocString(arena, strPntr)               NewStr((strPntr)->length,          AllocCharsAndFill  ((arena), (u64)(strPntr)->length, (strPntr)->chars))
#define NewStringInArena(arena, length, charPntr) NewStr((length),                   AllocCharsAndFill  ((arena), (u64)(length),          (charPntr)))
#define NewStringInArenaNt(arena, nullTermStr)    NewStr(strlen(nullTermStr), AllocCharsAndFillNt((arena), (nullTermStr)))

#define FreeString(arena, strPntr) do { NotNullStr(strPntr); if ((strPntr)->pntr != nullptr) { FreeMem((arena), (strPntr)->pntr, (strPntr)->length+1); (strPntr)->pntr = nullptr; (strPntr)->length = 0; } } while(0)

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
MyStr_t NewStr(const char* nullTermStr); //implemented in .cpp because it requires strlen
MyStr_t PrintInArenaStr(MemArena_t* arena, const char* formatString, ...);
bool StrEquals(MyStr_t left, MyStr_t right); //implemented in cpp because it requires memcmp
bool StrEquals(MyStr_t left, const char* rightNt); //implemented in cpp because it requires strlen
i32 StrCompareIgnoreCase(MyStr_t left, MyStr_t right, u32 compareLength, u32 leftOffset = 0, u32 rightOffset = 0);
i32 StrCompareIgnoreCase(MyStr_t left, MyStr_t right);
i32 StrCompareIgnoreCase(MyStr_t left, const char* rightNt);
// if extensionOut is not passed then the extension will be included in the fileNameOut
// Output MyStr_t are not reallocated so they are not all null-terminated
// extensionOut will include the '.' character
// If the path is actually a directory only we may interpret the last folder name as fileName w/ extension.
// Only use full file paths in order to avoid this, or have trailing "/"
void SplitFilePath(MyStr_t fullPath, MyStr_t* directoryOut, MyStr_t* fileNameOut, MyStr_t* extensionOut = nullptr);
bool FindSubstring(MyStr_t target, MyStr_t substring, u32* indexOut = nullptr, u32 startIndex = 0);

// +--------------------------------------------------------------+
// |                       Inline Functions                       |
// +--------------------------------------------------------------+
inline char GetLowercaseAnsiiChar(char c)
{
	if (c >= 'A' && c <= 'Z') { return 'a' + (c - 'A'); }
	return c;
}
inline char GetUppercaseAnsiiChar(char c)
{
	if (c >= 'a' && c <= 'z') { return 'A' + (c - 'a'); }
	return c;
}

inline u8 CharToU8(char c)
{
	return *((u8*)&c);
}
inline u32 CharToU32(char c)
{
	return (u32)(*((u8*)&c));
}

inline MyStr_t NewStr(u32 length, const char* chars)
{
	MyStr_t result;
	result.length = length;
	result.chars = (char*)chars;
	return result;
}

//A "Null Str" is one that has length but pntr is nullptr
inline bool IsNullStr(MyStr_t target)
{
	return (target.length > 0 && target.pntr == nullptr);
}
inline bool IsNullStr(const MyStr_t* targetPntr)
{
	return (targetPntr == nullptr || (targetPntr->length > 0 && targetPntr->pntr == nullptr));
}
inline bool IsEmptyStr(MyStr_t target)
{
	return (target.length == 0);
}
inline bool IsEmptyStr(const MyStr_t* targetPntr)
{
	NotNull(targetPntr);
	return (targetPntr->length == 0);
}
inline bool IsStrNullTerminated(MyStr_t target)
{
	if (target.chars == nullptr) { return false; }
	return (target.chars[target.length] == '\0');
}
inline bool IsStrNullTerminated(const MyStr_t* targetPntr)
{
	NotNull(targetPntr);
	if (targetPntr->chars == nullptr) { return false; }
	return (targetPntr->chars[targetPntr->length] == '\0');
}
inline bool BufferIsNullTerminated(u32 bufferSize, const char* bufferPntr)
{
	AssertIf(bufferSize > 0, bufferPntr != nullptr);
	for (u32 cIndex = 0; cIndex < bufferSize; cIndex++)
	{
		if (bufferPntr[cIndex] == '\0') { return true; }
	}
	return false;
}

inline MyStr_t StrSubstring(MyStr_t* target, u32 startIndex)
{
	NotNullStr(target);
	MyStr_t result;
	Assert(startIndex <= target->length);
	result.pntr = &target->chars[startIndex];
	result.length = target->length - startIndex;
	return result;
}
inline MyStr_t StrSubstring(MyStr_t* target, u32 startIndex, u32 endIndex)
{
	NotNullStr(target);
	MyStr_t result;
	Assert(startIndex <= target->length);
	Assert(endIndex >= startIndex);
	Assert(endIndex <= target->length);
	result.pntr = &target->chars[startIndex];
	result.length = endIndex - startIndex;
	return result;
}
inline MyStr_t StrSubstringLength(MyStr_t* target, u32 startIndex, u32 length)
{
	NotNullStr(target);
	MyStr_t result;
	Assert(startIndex + length <= target->length);
	result.pntr = &target->chars[startIndex];
	result.length = length;
	return result;
}

inline bool StrEqualsIgnoreCase(MyStr_t target, MyStr_t comparison)
{
	return (StrCompareIgnoreCase(target, comparison) == 0);
}
inline bool StrEqualsIgnoreCase(MyStr_t target, const char* comparisonNt)
{
	return (StrCompareIgnoreCase(target, comparisonNt) == 0);
}

inline MyStr_t GetFileNamePart(MyStr_t filePath, bool includeExtension = true)
{
	MyStr_t result;
	MyStr_t extensionThrowAway;
	SplitFilePath(filePath, nullptr, &result, (includeExtension ? nullptr : &extensionThrowAway));
	NotNullStr(&result);
	return result;
}

inline MyStr_t GetDirectoryPart(MyStr_t filePath)
{
	MyStr_t result;
	SplitFilePath(filePath, &result, nullptr, nullptr);
	NotNullStr(&result);
	return result;
}

inline const char* GetFileNamePartNt(const char* filePath)
{
	NotNull(filePath);
	MyStr_t result;
	SplitFilePath(NewStr(filePath), nullptr, &result);
	NotNullStr(&result);
	return result.chars;
}

#endif //  _MY_STR_H
