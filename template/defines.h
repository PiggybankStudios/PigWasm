/*
File:   defines.h
Author: Taylor Robbins
Date:   11\22\2022
This is #included by common.h
*/

#ifndef _DEFINES_H
#define _DEFINES_H

#define USE_CUSTOM_STD_LIBRARY 1
#define PRINT_BUFFER_SIZE      512 //chars

#define WASM_MEM_PAGE_SIZE     Kilobytes(4)

#define MAIN_HEAP_PAGE_SIZE      Kilobytes(128)
#define TEMP_ARENA_SIZE          Kilobytes(128)
#define TEMP_ARENA_MAX_NUM_MARKS 64

// #define PRINTF_DISABLE_SUPPORT_FLOAT

#endif //  _DEFINES_H
