#pragma once
#include "foundation\PxErrorCallback.h"


namespace Physics
{
	class CPhysXErrorHander : public physx::PxErrorCallback
	{
	public:
		CPhysXErrorHander();
		virtual ~CPhysXErrorHander();

		void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
	};
}
