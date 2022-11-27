/*
File:   common.h
Author: Taylor Robbins
Date:   11\22\2022
Description:
	** This should be included by every cpp file before any other includes
	** It in turn will include any commonly required header files
*/

#ifndef _COMMON_H
#define _COMMON_H

#include "build_defines_check.h"
#include "defines.h"

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "int_types.h"
#include "opengl.h"
#include "small_printf/printf.h"

#include "macros.h"
#include "assert.h"
#include "glue_api.h"

#endif //  _COMMON_H
