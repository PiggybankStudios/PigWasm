/*
File:   build_defines_check.h
Author: Taylor Robbins
Date:   11\22\2022
Description:
	** Makes sure that all the expected defines from the build.bat are present and that they are defined as boolean values
This is #included by common.h
*/

#ifndef _BUILD_DEFINES_CHECK_H
#define _BUILD_DEFINES_CHECK_H

#if !defined(WINDOWS_COMPILATION) && !defined(OSX_COMPILATION) && !defined(LINUX_COMPILATION) && !defined(WASM_COMPILATION)
#error Either WINDOWS_COMPILATION, OSX_COMPILATION, LINUX_COMPILATION, or WASM_COMPILATION must be defined before including anything from Common
#endif

#ifdef WASM_COMPILATION
#undef WASM_COMPILATION
#define WASM_COMPILATION 1
#else
#error We only support compiling in WASM_COMPILATION mode right now
#endif

#ifndef ASSERTIONS_ENABLED
#error ASSERTIONS_ENABLED was not defined! Please define this through compiler options like -D
#endif

#endif //  _BUILD_DEFINES_CHECK_H
