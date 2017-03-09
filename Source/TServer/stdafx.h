// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers



// TODO: reference additional headers your program requires here
#include "../CommonUtilities/GrowingArray.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/vector3.h"
#include "../CommonUtilities/vector2.h"
#include "../CommonUtilities/CUTime.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr
#endif // !SAFE_DELETE