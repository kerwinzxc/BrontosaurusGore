// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr

#include <utility>

#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/GrowingArray.h"
#include "../CommonUtilities/StaticArray.h"
#include "../CommonUtilities/VectorOnStack.h"
#include "../CommonUtilities/CUTime.h"
//#include "../CommonUtilities/MemoryLeekLogger.h"

#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Event.h"
#include "../PostMaster/Message.h"

#include "State.h"

// TODO: reference additional headers your program requires here
