#pragma once

#include "targetver.h"

#include <string>

#include "../CommonUtilities/GrowingArray.h"
#include "../CommonUtilities/CommandLineManager.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/MemoryLeekLogger.h"
#include "../CommonUtilities/CUTime.h"

#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/Event.h"

#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/EDebugInfoDrawerFlags.h"
#include "../BrontosaurusEngine/Scene.h"
#include "../BrontosaurusEngine/ModelInstance.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr
#endif

#define SUPRESS_UNUSED_WARNING(variable) variable;
