#include "stdafx.h"
#include "LoadPointLightComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "PointLightComponent.h"
#include "PointLightComponentManager.h"

int LoadPointLightComponent(KLoader::SLoadedComponentData someData)
{
	PointLightComponent* pointLight = CPointLightComponentManager::GetInstance().CreateAndRegisterComponent();
	pointLight->SetColor(someData.myData.at("Color").GetVector3f());
	pointLight->SetIntensity(someData.myData.at("Intensity").GetFloat());
	pointLight->SetRange(someData.myData.at("Range").GetFloat());
	return pointLight->GetId();
}
