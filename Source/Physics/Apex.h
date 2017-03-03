#pragma once
#include "ApexResourceCallback.h"

namespace physx
{
	class PxPhysics;
	class PxCooking;
	class PxPvd;
	class PxFoundation;
}

namespace nvidia
{
	namespace apex
	{
		class ApexSDK;
		class ModuleDestructible;
		class ModuleParticles;
		class UserRenderResourceManager;
		class ResourceCallback;
		enum ApexCreateError;
	}
}

namespace Physics
{
	class CApex
	{
	public:
		struct SApexInit
		{
			physx::PxPhysics* physics;
			physx::PxCooking* cooking;
			physx::PxPvd* pvd;
			physx::PxFoundation* foundation;
		};

		bool CheckError(nvidia::ApexCreateError aError);
		explicit CApex(const SApexInit& anApexInit);
		~CApex();
	protected:
		const std::wstring GetErrorString(nvidia::ApexCreateError aError);

		nvidia::apex::ApexSDK* myApexSDK;
		nvidia::apex::ModuleDestructible* myModulieDestructible;
		nvidia::apex::ModuleParticles* myModuleParticle;
		nvidia::apex::UserRenderResourceManager* myRenderResourceManager;
		nvidia::apex::ResourceCallback* myResourceCallback;
	};
}

