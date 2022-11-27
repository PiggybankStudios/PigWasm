/*
File:   math.h
Author: Taylor Robbins
Date:   11\22\2022
*/

#ifndef _MATH_H
#define _MATH_H

// +--------------------------------------------------------------+
// |                      Builtin Functions                       |
// +--------------------------------------------------------------+
#define NAN       __builtin_nanf("")
#define INFINITY  __builtin_inff()

//TODO: Are all these functions builtin when compiling for desktop without stdlib?
//      (When compiling for WASM we either get the builtin instructions or we import implementations from javascript)
inline float  fminf(float value1, float value2)  { return __builtin_fminf(value1, value2); }
inline double fmin(double value1, double value2) { return __builtin_fmin(value1, value2);  }
inline float  fmaxf(float value1, float value2)  { return __builtin_fmaxf(value1, value2); }
inline double fmax(double value1, double value2) { return __builtin_fmax(value1, value2);  }

inline float       fabsf(float value)                          { return __builtin_fabsf(value);          }
inline double      fabs(double value)                          { return __builtin_fabs(value);           }
inline long double fabsl(long double value)                    { return __builtin_fabsl(value);          }
inline float       fmodf(float numer, float denom)             { return __builtin_fmodf(numer, denom);   }
inline double      fmod(double numer, double denom)            { return __builtin_fmod(numer, denom);    }
inline long double fmodl(long double numer, long double denom) { return __builtin_fmodl(numer, denom);   }

inline float  roundf(float value)                { return __builtin_roundf(value);         }
inline double round(double value)                { return __builtin_round(value);          }
inline float  floorf(float value)                { return __builtin_floorf(value);         }
inline double floor(double value)                { return __builtin_floor(value);          }
inline float  ceilf(float value)                 { return __builtin_ceilf(value);          }
inline double ceil(double value)                 { return __builtin_ceil(value);           }

inline float  sinf(float value)                  { return __builtin_sinf(value);           }
inline double sin(double value)                  { return __builtin_sin(value);            }
inline float  asinf(float value)                 { return __builtin_asinf(value);          }
inline double asin(double value)                 { return __builtin_asin(value);           }
inline float  cosf(float value)                  { return __builtin_cosf(value);           }
inline double cos(double value)                  { return __builtin_cos(value);            }
inline float  acosf(float value)                 { return __builtin_acosf(value);          }
inline double acos(double value)                 { return __builtin_acos(value);           }
inline float  tanf(float value)                  { return __builtin_tanf(value);           }
inline double tan(double value)                  { return __builtin_tan(value);            }
inline float  atanf(float value)                 { return __builtin_atanf(value);          }
inline double atan(double value)                 { return __builtin_atan(value);           }
inline float  atan2f(float numer, float denom)   { return __builtin_atan2f(numer, denom);  }
inline double atan2(double numer, double denom)  { return __builtin_atan2(numer, denom);   }

inline float  powf(float value, float exponent)  { return __builtin_powf(value, exponent); }
inline double pow(double value, double exponent) { return __builtin_pow(value, exponent);  }
inline float  sqrtf(float value)                 { return __builtin_sqrtf(value);          }
inline double sqrt(double value)                 { return __builtin_sqrt(value);           }
inline float  cbrtf(float value)                 { return __builtin_cbrtf(value);          }
inline double cbrt(double value)                 { return __builtin_cbrt(value);           }

inline float  logf(float value)                  { return __builtin_logf(value);           }
inline double log(double value)                  { return __builtin_log(value);            }
inline float  log2f(float value)                 { return __builtin_log2f(value);          }
inline double log2(double value)                 { return __builtin_log2(value);           }
inline float  log10f(float value)                { return __builtin_log10f(value);         }
inline double log10(double value)                { return __builtin_log10(value);          }

//Scales x by FLT_RADIX raised to the power of n
inline float       scalbnf(float value, int power)       { return __builtin_scalbnf(value, power); }
inline double      scalbn(double value, int power)       { return __builtin_scalbn(value, power);  }
inline long double scalbnl(long double value, int power) { return __builtin_scalbnl(value, power); }

//Composes a floating point value with the magnitude of x and the sign of y
inline float       copysignf(float magnitude, float sign)             { return __builtin_copysignf(magnitude, sign); }
inline double      copysign(double magnitude, double sign)            { return __builtin_copysign(magnitude, sign);  }
inline long double copysignl(long double magnitude, long double sign) { return __builtin_copysignl(magnitude, sign); }

#endif //  _MATH_H
