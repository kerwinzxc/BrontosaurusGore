#include "stdafx.h"
#include "LoadModel.h"
#include "Component.h"
#include "ModelComponentManager.h"

int LoadMeshFilter(KLoader::SLoadedComponentData someData)
{
	CComponent* const modelComp = CModelComponentManager::GetInstance().CreateComponent(someData.myData.at("meshPath").GetString().c_str());
	return modelComp->GetId();
}
