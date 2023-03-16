// helper file for defining wchar for other than Windows based environments
#pragma once
#include <cstdint>

#ifndef _NATIVE_WCHAR_T_DEFINED
typedef uint16_t WCHAR;
#else
typedef wchar_t WCHAR;
#endif