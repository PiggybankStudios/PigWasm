/*
File:   stdlib.h
Author: Taylor Robbins
Date:   12\01\2022
*/

#ifndef _STDLIB_H
#define _STDLIB_H

#ifndef NULL
#define NULL nullptr
#endif

inline int  abs(int value)   { return __builtin_abs(value);  }
inline long labs(long value) { return __builtin_labs(value); }

long strtol(const char* string, char** endptr, int base)
{
	NotNull(string);
	MyStr_t str = NewStr(strlen(string), string);
	// TrimWhitespace(&str); //TODO: Should we eat whitespace?
	u64 resultU64 = 0;
	bool foundHexDesignation = (base == 16);
	bool foundBinaryDesignation = (base == 2);
	Assert(foundHexDesignation || foundBinaryDesignation || base == 10);
	
	bool isNegative = false;
	if (str.length >= 1 && str.chars[0] == '-') { isNegative = true; str = StrSubstring(&str, 1); }
	if (str.length >= 1 && str.chars[0] == '+') { isNegative = false; str = StrSubstring(&str, 1); }
	
	bool foundNumbers = false;
	while (str.length > 0)
	{
		u32 codepoint = CharToU32(str.chars[0]);
		
		if (foundHexDesignation && codepoint >= '0' && codepoint <= '9')
		{
			if (resultU64 > UINT64_MAX / 16ULL)
			{
				return 0;
			}
			resultU64 = resultU64 * 16ULL + (u64)(codepoint - '0');
			foundNumbers = true;
		}
		else if (foundHexDesignation && codepoint >= 'A' && codepoint <= 'F')
		{
			if (resultU64 > UINT64_MAX / 16ULL)
			{
				return 0;
			}
			resultU64 = resultU64 * 16ULL + (u64)((codepoint - 'A') + 10);
			foundNumbers = true;
		}
		else if (foundHexDesignation && codepoint >= 'a' && codepoint <= 'f')
		{
			if (resultU64 > UINT64_MAX / 16ULL)
			{
				return 0;
			}
			resultU64 = resultU64 * 16ULL + (u64)((codepoint - 'a') + 10);
			foundNumbers = true;
		}
		else if (foundBinaryDesignation && codepoint >= '0' && codepoint <= '1')
		{
			if (resultU64 > UINT64_MAX / 2ULL)
			{
				return 0;
			}
			resultU64 = resultU64 * 2ULL + (u64)((codepoint == '1') ? 1 : 0);
			foundNumbers = true;
		}
		else if (!foundHexDesignation && !foundBinaryDesignation && codepoint >= '0' && codepoint <= '9')
		{
			if (resultU64 > UINT64_MAX / 10ULL)
			{
				return 0;
			}
			resultU64 = resultU64 * 10ULL + (u64)(codepoint - '0');
			foundNumbers = true;
		}
		else if (codepoint == ' ')
		{
			//do nothing, eat whitespace
		}
		else 
		{
			return 0;
		}
		
		str.chars++;
		str.length--;
	}
	
	if (!foundNumbers) { return 0; }
	
	return (i64)resultU64 * (i64)(isNegative ? -1 : 1);
}

#endif //  _STDLIB_H
