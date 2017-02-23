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
	pointLight->SetColor(CU::Vector3f(someData.myData.at("color").GetVector4f()) / 255.f);
	pointLight->SetIntensity(someData.myData.at("intensity").GetFloat());
	pointLight->SetRange(someData.myData.at("range").GetFloat());
	return pointLight->GetId();
}
