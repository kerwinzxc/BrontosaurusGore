#include "stdafx.h"
#include "LoadPointLightComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "PointLightComponent.h"
#include "PointLightComponentManager.h"
#include "GameObject.h"

int LoadPointLightComponent(KLoader::SLoadedComponentData someData)
{

	if (someData.myData.HasKey("range"))
	{
		PointLightComponent* pointLight = CPointLightComponentManager::GetInstance().CreateAndRegisterComponent();
		pointLight->SetColor(CU::Vector3f(someData.myData.at("color").GetVector4f("xyzw") / 255.f));
		pointLight->SetIntensity(someData.myData.at("intensity").GetFloat());
		pointLight->SetRange(someData.myData.at("range").GetFloat());
		return pointLight->GetId();
	}
	else
	{
		int id = KLoader::CKevinLoader::GetInstance().GetCurrentObjectIndex();
		CGameObject* parent = reinterpret_cast<CGameObject*>(CComponentManager::GetInstance().GetComponent(id));
		Lights::SDirectionalLight directionalLight;
		directionalLight.direction = parent->GetToWorldTransform().myForwardVector;
		directionalLight.intensity = someData.myData.at("intensity").GetFloat();
		directionalLight.color = someData.myData.at("color").GetVector4f("xyzw") / 255.f;
		CPointLightComponentManager::GetInstance().AddDirectionalLightToScene(directionalLight);

		return NULL_COMPONENT;
	}
}
