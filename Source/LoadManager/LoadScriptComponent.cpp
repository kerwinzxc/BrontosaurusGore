#include "stdafx.h"
#include "LoadScriptComponent.h"

#include "ScriptComponentManager.h"
#include "ScriptComponent.h"

int LoadScriptComponent(KLoader::SLoadedComponentData someData)
{
	CScriptComponentManager* scriptComponentManager = CScriptComponentManager::GetInstance();

	const std::string& scriptPath = someData.myData["FilePath"].GetString();

	CScriptComponent* component = scriptComponentManager->CreateComponent(scriptPath);
	
	return (component) ? component->GetId() : NULL_COMPONENT;
}
