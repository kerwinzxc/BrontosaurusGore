#pragma once
#include "PointLight.h"
#include "Lights.h"
#include <unordered_map>

class CLightManager
{
public:
	CPointLight* LoadLight(ePointLights aLightType); //overload later
	CLightManager();
	~CLightManager();
private:
	CPointLight* CreateLight(ePointLights aLightType); //later return parent light.

	std::unordered_map<ePointLights, CPointLight*> myPointLights;
};

