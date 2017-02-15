#include "stdafx.h"
#include "FontEngineFacade.h"
#include "FontEngine.h"

CFontEngine* CFontEngineFacade::ourInstance = nullptr;

CFontEngineFacade::CFontEngineFacade()
{
}


CFontEngineFacade::~CFontEngineFacade()
{
}

void CFontEngineFacade::CreateInstance()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CFontEngine;
		ourInstance->Init();
	}
}

CFontEngineFacade CFontEngineFacade::GetInstance()
{
	return CFontEngineFacade();
}

void CFontEngineFacade::DestroyInstance()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
	}
}

void CFontEngineFacade::Init() const
{
	ourInstance->Init();
}

CFT_FontFacade CFontEngineFacade::GetFace(const char* aFilePath) const
{
	return CFT_FontFacade(ourInstance->GetFace(aFilePath));
}
