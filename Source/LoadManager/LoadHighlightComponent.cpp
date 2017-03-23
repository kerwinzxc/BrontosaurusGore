#include "stdafx.h"
#include "LoadHighlightComponent.h"
#include "HighlightComponent.h"

int LoadHighlightComponent(KLoader::SLoadedComponentData someData)
{

	CHighlightComponent* highlight = new CHighlightComponent;
	highlight->SetColor(someData.myData["colour"].GetVector4f("rgba"));
	highlight->SetIntensity(someData.myData["intensity"].GetFloat());
	CComponentManager::GetInstance().RegisterComponent(highlight);

	return highlight->GetId();
}
