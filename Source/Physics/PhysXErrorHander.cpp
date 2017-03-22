#include "stdafx.h"
#include "PhysXErrorHander.h"



namespace Physics
{
	CPhysXErrorHander::CPhysXErrorHander()
	{
	}

	CPhysXErrorHander::~CPhysXErrorHander()
	{
	}

	void CPhysXErrorHander::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		//TODO: Put this back
		//PHYSICS_LOG("\nError %i: \n\t %s \n\t File: %s Line: %i \n\n", code, message, file, line);
		//DL_ASSERT("");
	}
}