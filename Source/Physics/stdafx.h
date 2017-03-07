// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(comptr) if (comptr != nullptr) { comptr->release(); comptr = nullptr; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(POINTER) delete POINTER; POINTER = nullptr
#endif

#include <utility>

#include "../CommonUtilities/GrowingArray.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/MemoryLeekLogger.h"
#include "../CommonUtilities/matrix44.h"


