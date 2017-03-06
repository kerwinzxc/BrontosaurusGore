#include "stdafx.h"
#include "Apex.h"
#include "ApexSDK.h"
#include "Include/apex_1.4/Apex.h"
#include "destructible/ModuleDestructible.h"
#include "particles/ModuleParticles.h"
#include "ApexRenderResourceManager.h"
#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/DXFramework.h"
#include "ApexResourceCallback.h"
#include <foundation/PxErrorCallback.h>

bool Physics::CApex::CheckError(nvidia::ApexCreateError aError)
{
	if(aError != nvidia::APEX_CE_NO_ERROR)
	{
		DL_MESSAGE_BOX((std::wstring(L"Apex error: ") + GetErrorString(aError)).c_str());
		return false;
	}

	return true;
}

Physics::CApex::CApex(const SApexInit& anApexInit)
{
	nvidia::ApexSDKDesc desc;
	desc.physXSDK = anApexInit.physics;
	desc.cooking = anApexInit.cooking;
	desc.pvd = anApexInit.pvd;
	desc.foundation = anApexInit.foundation;

	myRenderResourceManager = new Apex::CApexRenderResourceManager(CEngine::GetInstance()->GetFramework()->GetDevice());
	desc.renderResourceManager = myRenderResourceManager;

	myResourceCallback = new Apex::CApexResourceCallback();
	desc.resourceCallback = myResourceCallback;

	nvidia::ApexCreateError error;
	myApexSDK = nvidia::CreateApexSDK(desc, &error);
	PX_ASSERT(myApexSDK);

	myModulieDestructible = static_cast<nvidia::ModuleDestructible*>(myApexSDK->createModule("Destructible"));
	if (CheckError(error))
	{
		NvParameterized::Interface* params = myModulieDestructible->getDefaultModuleDesc();
		myModulieDestructible->init(*params);
	}


	myModuleParticle = static_cast<nvidia::ModuleParticles*>(myApexSDK->createModule("Particles",&error));
	
	if(CheckError(error))
	{
		NvParameterized::Interface* params = myModuleParticle->getDefaultModuleDesc();
		myModuleParticle->init(*params);
	}

	int i = 0;
}

Physics::CApex::~CApex()
{
}

const std::wstring Physics::CApex::GetErrorString(nvidia::ApexCreateError aError)
{
	std::wstring result;
	switch (aError)
	{
	case nvidia::APEX_CE_NO_ERROR: 
		result = L"No errors occurred when creating the Physics SDK.";
		break;
	case nvidia::APEX_CE_NOT_FOUND: 
		result = L"Unable to find the libraries.";
		break;
	case nvidia::APEX_CE_WRONG_VERSION: 
		result = L"The application supplied a version number that does not match with the libraries.";
		break;
	case nvidia::APEX_CE_DESCRIPTOR_INVALID: 
		result = L"The supplied descriptor is invalid.";
		break;
	case nvidia::APEX_CE_CREATE_NO_ALLOWED: 
		result = L"This module cannot be created by the application, it is created via a parent module use as APEX_Particles or APEX_Legacy.";
		break;
	default: 
		result = L"Unknow error.";
		break;
	}

	return result;
}
