/*
File:   macros.h
Author: Taylor Robbins
Date:   11\27\2022
*/

#ifndef _MACROS_H
#define _MACROS_H

// +--------------------------------------------------------------+
// |                       Global Constants                       |
// +--------------------------------------------------------------+
//Actual Value of Pi:  3.1415926535897932384626433832795...
#define Pi64           3.14159265358979311599796346854      //accurate to 15 digits
#define Pi32           3.1415927410125732421875f            //accurate to 6 digits
#define QuarterPi64    (Pi64/4.0)
#define ThirdPi64      (Pi64/3.0)
#define HalfPi64       (Pi64/2.0)
#define ThreeHalfsPi64 (Pi64*(3.0/2.0))
#define TwoPi64        (2*Pi64)
#define QuarterPi32    (Pi32/4.0f)
#define ThirdPi32      (Pi32/3.0f)
#define HalfPi32       (Pi32/2.0f)
#define ThreeHalfsPi32 (Pi32*(3.0f/2.0f))
#define TwoPi32        (2*Pi32)
//Actual Value of e:   2.7182818284590452353602874713526...
#define e64            2.71828182845904509079559829843      //accurate to 15 digits
#define e32            2.71828174591064453125f              //accurate to 6 digits

// +--------------------------------------------------------------+
// |                     Function-like Macros                     |
// +--------------------------------------------------------------+
#define EXPORTED_FUNC(returnType, functionName, ...) extern "C" returnType __attribute__((export_name(#functionName))) functionName(__VA_ARGS__)
#define UNUSED(varName)        (void)(varName)
#define UNREFERENCED(varName)  (void)(varName)

#define Increment(variable, max)           if ((variable) < (max)) { (variable)++; } else { (variable) = (max); }
#define IncrementU8(variable)              if ((variable) < 0xFF) { (variable)++; } else { (variable) = 0xFF; }
#define IncrementU16(variable)             if ((variable) < 0xFFFF) { (variable)++; } else { (variable) = 0xFFFF; }
#define IncrementU32(variable)             if ((variable) < 0xFFFFFFFFUL) { (variable)++; } else { (variable) = 0xFFFFFFFFUL; }
#define IncrementU64(variable)             if ((variable) < 0xFFFFFFFFFFFFFFFFULL) { (variable)++; } else { (variable) = 0xFFFFFFFFFFFFFFFFULL; }
#define IncrementBy(variable, amount, max) if ((variable) + (amount) < (max) && (variable) + (amount) > (variable)) { (variable) += (amount); } else { (variable) = (max); }
#define IncrementU8By(variable, amount)    if ((variable) + (amount) < 0xFF && (variable) + (amount) > (variable)) { (variable) += (amount); } else { (variable) = 0xFF; }
#define IncrementU16By(variable, amount)   if ((variable) + (amount) < 0xFFFF && (variable) + (amount) > (variable)) { (variable) += (amount); } else { (variable) = 0xFFFF; }
#define IncrementU32By(variable, amount)   if ((variable) + (amount) < 0xFFFFFFFFUL && (variable) + (amount) > (variable)) { (variable) += (amount); } else { (variable) = 0xFFFFFFFFUL; }
#define IncrementU64By(variable, amount)   if ((variable) + (amount) < 0xFFFFFFFFFFFFFFFFULL && (variable) + (amount) > (variable)) { (variable) += (amount); } else { (variable) = 0xFFFFFFFFFFFFFFFFULL; }
#define Decrement(variable)                if ((variable) > 0) { (variable)--; } else { (variable) = 0; }
#define DecrementBy(variable, amount)      if ((variable) >= (amount)) { (variable) -= (amount); } else { (variable) = 0; }

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define IsFlagSet(BitwiseField, Bit) (((BitwiseField) & (Bit)) != 0)
#define FlagSet(BitwiseField, Bit)   (BitwiseField) |= (Bit)
#define FlagUnset(BitwiseField, Bit) (BitwiseField) &= ~(Bit)
#define FlagToggle(BitwiseField, Bit) ((BitwiseField) ^= (Bit))
#define FlagSetTo(BitwiseField, Bit, condition) if (condition) { FlagSet((BitwiseField), (Bit)); } else { FlagUnset((BitwiseField), (Bit)); }

#define Kilobytes(value) ((value) * 1024UL)
#define Megabytes(value) (Kilobytes((value)) * 1024UL)
#define Gigabytes(value) (Megabytes((value)) * 1024UL)

#define ToRadians32(degrees)		((degrees)/180.0f * Pi32)
#define ToRadians64(degrees)		((degrees)/180.0 * Pi64)
#define ToDegrees32(radians)		((radians)/Pi32 * 180.0f)
#define ToDegrees64(radians)		((radians)/Pi64 * 180.0)

#define ClearArray(Array)      memset((Array), '\0', sizeof((Array)))
#define ClearStruct(Structure) memset(&(Structure), '\0', sizeof((Structure)))
#define ClearPointer(Pointer)  memset((Pointer), '\0', sizeof(*(Pointer)));

#endif //  _MACROS_H
