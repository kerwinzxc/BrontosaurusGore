#pragma once

#include "../CommonUtilities/GrowingArray.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/VectorOnStack.h"
#include "../CommonUtilities/StaticMemoryPool.h"

#include "Intersection.h"

#include "SquareCollider.h"
#include "CircleCollider.h"
#include "GroupCollider.h"
#include "PointCollider.h"
#include "TriangleCollider.h"

#include "CollisionRenderer.h"
#include "CollisionRenderMessages.h"

#define SUPRESS_UNUSED_WARNING(variable) variable
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr
#define self (*this)
