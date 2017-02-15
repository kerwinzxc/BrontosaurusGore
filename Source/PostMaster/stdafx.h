// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <utility>
#include <map>

#include "MessageType.h"
#include "EMessageReturn.h"

#include "../CommonUtilities/GrowingArray.h"
#include "../CommonUtilities/StaticArray.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/vector2.h"

#define IntCast(eMsg) static_cast<unsigned int>(eMsg)

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr
#endif

#define SUPRESS_UNUSED_WARNING(variable) variable;

// TODO: reference additional headers your program requires here
