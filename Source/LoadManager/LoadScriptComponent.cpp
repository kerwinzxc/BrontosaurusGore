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

	CComponent* component = scriptComponentManager->CreateAbstractComponent(scriptPath);
	if (!component)
	{
		return NULL_COMPONENT;
	}

	return component->GetId();
}
