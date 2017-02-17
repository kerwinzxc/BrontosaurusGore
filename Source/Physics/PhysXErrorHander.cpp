#include "stdafx.h"
#include "PhysXErrorHander.h"


CPhysXErrorHander::CPhysXErrorHander()
{
}


CPhysXErrorHander::~CPhysXErrorHander()
{
}

void CPhysXErrorHander::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
{
	PHYSICS_LOG("Error %i: \n\t %s \n\t File: %s Line: %i \n\n", code, message, file, line);
	DL_ASSERT("");
}
