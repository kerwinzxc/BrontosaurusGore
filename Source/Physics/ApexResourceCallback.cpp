#include "stdafx.h"
#include "ApexResourceCallback.h"


Physics::Apex::CApexResourceCallback::CApexResourceCallback()
{
}


Physics::Apex::CApexResourceCallback::~CApexResourceCallback()
{
}

void* Physics::Apex::CApexResourceCallback::requestResource(const char* nameSpace, const char* name)
{
	return nullptr;
}

void Physics::Apex::CApexResourceCallback::releaseResource(const char* nameSpace, const char* name, void* resource)
{
}
