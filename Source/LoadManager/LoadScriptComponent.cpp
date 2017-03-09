#include "stdafx.h"
#include "LoadScriptComponent.h"

#include "ScriptComponentManager.h"
#include "ScriptComponent.h"

int LoadScriptComponent(KLoader::SLoadedComponentData someData)
{
	CScriptComponentManager* scriptComponentManager = CScriptComponentManager::GetInstance();
	if (!scriptComponentManager)
	{
		return NULL_COMPONENT;
	}

	const std::string& scriptPath = someData.myData["FilePath"].GetString();

	CScriptComponent* component = scriptComponentManager->CreateComponent(scriptPath);
	if (!component)
	{
		return NULL_COMPONENT;
	}

	return component->GetId();
}
