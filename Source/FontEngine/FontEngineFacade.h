#pragma once
#include "FT_FontFacade.h"

class CFontEngine;
class CFontEngineFacade
{
public:
	CFontEngineFacade();
	~CFontEngineFacade();

	static void CreateInstance();
	static CFontEngineFacade GetInstance();
	static void DestroyInstance();
	static bool GetIsCreated();

	void Init() const;
	CFT_FontFacade GetFace(const char* aFilePath) const;
private:

	static CFontEngine* ourInstance;
};

