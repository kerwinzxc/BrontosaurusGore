// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#ifdef _WIN32
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr

#include <utility>
#include <string>
#include <string.h>
#include <cstdint>
#include <stdlib.h>
#include <fstream>

#include "DL_Debug.h"
#include "EMouseButtons.h"
#include "GrowingArray.h"

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix.h"

#include "MemoryLeekLogger.h"

// TODO: reference additional headers your program requires here
