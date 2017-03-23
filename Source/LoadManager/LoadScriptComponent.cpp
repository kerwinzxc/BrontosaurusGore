#include "stdafx.h"
#include "LoadScriptComponent.h"

#include "ScriptComponentManager.h"

int LoadScriptComponent(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	
	CScriptComponentManager* scriptComponentManager = loadManager.GetCurrentPLaystate().GetScriptComponentManager();
	if (!scriptComponentManager)
	{
		return NULL_COMPONENT;
	}

	const std::string& scriptPath = someData.myData["FilePath"].GetString();
	std::map<std::string, std::string> dataMap;

	if(someData.myData.HasKey("Data"))
	{
		CU::CJsonValue dataArray = someData.myData["Data"];

		for (int i = 0; i < dataArray.Size(); ++i)
		{
			CU::CJsonValue elementObject = dataArray[i];
			dataMap[elementObject["key"].GetString()] = elementObject["value"].GetString();
		}
	}
	
	CComponent* component = scriptComponentManager->CreateAbstractComponent(scriptPath, dataMap);
	if (!component)
	{
		return NULL_COMPONENT;
	}

	return component->GetId();
}
