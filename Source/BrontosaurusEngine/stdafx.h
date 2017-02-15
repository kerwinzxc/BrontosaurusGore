// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <utility>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <fstream>

#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/GrowingArray.h"
#include "../CommonUtilities/StaticArray.h"
#include "../CommonUtilities/VectorOnStack.h"
#include "../CommonUtilities/DynamicString.h"
#include "../CommonUtilities/StringHelper.h"

#include "../CommonUtilities/MemoryLeekLogger.h"
#include "../CommonUtilities/TimerManager.h"

#include "../CommonUtilities/vector2.h"
#include "../CommonUtilities/vector3.h"
#include "../CommonUtilities/vector4.h"
#include "../CommonUtilities/matrix33.h"
#include "../CommonUtilities/matrix44.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr
#endif

#define self (*this)

#define SAFE_RELEASE(comptr) if (comptr != nullptr) { comptr->Release(); comptr = nullptr; }
#define SAFE_ADD_REF(com_ptr) if (com_ptr) { com_ptr->AddRef(); }

#ifndef SUPRESS_UNUSED_WARNING
#define SUPRESS_UNUSED_WARNING(variable) variable
#endif // !SUPRESS_UNUSED_WARNING

#include "Engine.h"
#include "DXFramework.h"

// TODO: reference additional headers your program requires here
