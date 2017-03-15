#pragma once
#include "stdafx.h"
#include "GlobalLuaFunctions.h"
#include "ScriptHelperFunctions.h"

#include "../LuaWrapper/SSlua/SSlua.h"
#include "../LuaWrapper/SSArgument/SSArgument.h"

#include "../Components/ComponentMessage.h"
#include "../Components/ComponentManager.h"
#include "../Components/Component.h"
#include "../Components/GameObject.h"
#include "../Components/ComponentMessageCallback.h"

#include "ComponentMessageTypeToLua.h"
#include "ParticleEffectManager.h"

#include "../CommonUtilities/JsonValue.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../PostMaster/ChangeLevel.h"
#include "PollingStation.h"

#define GLOBAL_LUA_FUNCTION_ERROR DL_MESSAGE_BOX
#define RETURN_VOID() return SSlua::ArgumentList()
#define RETURN_ZERO() return SSlua::ArgumentList({ SSArgument(ssLuaNumber(0.0)) })
#define PAPA CGameObject* parent

SSlua::ArgumentList ComponentGetParent(const SSlua::ArgumentList& aArgumentList)
{
	CComponentManager* componentManager = CComponentManager::GetInstancePtr();
	if (!componentManager)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetParent, component manager not created");
		RETURN_ZERO();
	}

	if (!ScriptHelper::AssertArgumentCount("GetParent", 1, aArgumentList.Size(), true))
	{
		RETURN_ZERO();
	}

	ComponentId componentID = NULL_COMPONENT;
	CComponent* component = nullptr;
	if (aArgumentList.GetFirst().GetType() == eSSType::LIGHTUSERDATA)
	{
		component = static_cast<CComponent*>(aArgumentList.GetFirst().GetUserData());
	}
	else if (aArgumentList.GetFirst().GetType() == eSSType::NUMBER)
	{
		componentID = aArgumentList[0].GetUInt();
		if (componentID != NULL_COMPONENT)
		{
			component = componentManager->GetComponent(componentID);
		}
	}
	else
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetParent, expected component pointer (light userdata) or component id (number), got %s", aArgumentList.GetFirst().GetTypeName());
		RETURN_ZERO();
	}

	if (!component)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetParent, component id %u represented NULL", componentID);
		RETURN_ZERO();
	}

	PAPA = component->GetParent();
	if (!parent)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetParent, parent of component with id %u was NULL", componentID);
		RETURN_ZERO();
	}

	ComponentId parentID = parent->GetId();
	if (parentID == NULL_COMPONENT)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetParent, parent of component with id %u was not registered", componentID);
		RETURN_ZERO();
	}

	SSlua::ArgumentList returnValues(1u);

	returnValues.Add(SSArgument(ssLuaNumber(static_cast<ssLuaNumber>(parentID))));
	return returnValues;
}

SSlua::ArgumentList GameObjectGetPosition(const SSlua::ArgumentList& aArgumentList)
{
	CComponentManager* componentManager = CComponentManager::GetInstancePtr();
	if (!componentManager)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetPosition, component manager not created");
		return{ ssLuaNumber(0.0), ssLuaNumber(0.0) , ssLuaNumber(0.0) };
	}

	if (!ScriptHelper::AssertArgumentList("GetPosition", { eSSType::NUMBER }, aArgumentList, true))
	{
		return{ ssLuaNumber(0.0), ssLuaNumber(0.0) , ssLuaNumber(0.0) };
	}

	ComponentId gameObjectID = aArgumentList[0].GetUInt();
	CComponent* gameObjectComponent = componentManager->GetComponent(gameObjectID);
	if (!gameObjectComponent)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetPosition, it was called on object that was null");
		return{ ssLuaNumber(0.0), ssLuaNumber(0.0) , ssLuaNumber(0.0) };
	}

	if (!gameObjectComponent->IsGameObject())
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetPosition, it was called on object that is not a gameobject");
		return{ ssLuaNumber(0.0), ssLuaNumber(0.0) , ssLuaNumber(0.0) };
	}

	CGameObject* gameObject = static_cast<CGameObject*>(gameObjectComponent);
	CU::Vector3f gameObjectPosition(gameObject->GetWorldPosition());
	
	SSlua::ArgumentList position(3);
		
	position.Add(ssLuaNumber(gameObjectPosition.x));
	position.Add(ssLuaNumber(gameObjectPosition.y));
	position.Add(ssLuaNumber(gameObjectPosition.z));

	return position;
}

SSlua::ArgumentList ComponentNotifyParent(const SSlua::ArgumentList& aArgumentList)
{
	"Third argument (optional): the data you want to send (number, string, vector2/3 whatever)";

	CComponentManager* componentManager = CComponentManager::GetInstancePtr();
	if (!componentManager)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetParent, component manager not created");
		RETURN_VOID();
	}
	
	if (!ScriptHelper::AssertArgumentList("NotifyParent", { eSSType::NUMBER, eSSType::NUMBER }, aArgumentList, false))
	{
		RETURN_VOID();
	}

	ComponentId id = aArgumentList[0].GetUInt();

	CComponent* component = componentManager->GetComponent(id);
	if (!component)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in NotifyParent, component id %u represented NULL", id);
		RETURN_VOID();
	}

	int messageType = aArgumentList[1].GetInt();
	if (messageType >= static_cast<int>(eComponentMessageType::eLength))
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in NotifyParent, invalid message type %d", messageType);
		RETURN_VOID();
	}

	eComponentMessageType messageTypeE = static_cast<eComponentMessageType>(messageType);
	SComponentMessageData messageData;

	if (aArgumentList.Size() > 2u)
	{
		const SSArgument& thirdArgument = aArgumentList[2u];

		if (messageTypeE == eComponentMessageType::eOnCollisionEnter && thirdArgument.GetType() == eSSType::NUMBER)
		{
			messageData.myComponent = componentManager->GetComponent(thirdArgument.GetUInt());
			messageData.myGameObject->GetName();
		}
		else
		{
			std::string typeName("unknown");
			ComponentMessage::GetTypeName(messageTypeE, typeName);
			DL_MESSAGE_BOX("Third argument is not implemented for %s and %s. This is fine but you may not get the results you expect mvh carl", typeName.c_str(), thirdArgument.GetTypeName());
		}
	}

	component->NotifyParent(messageTypeE, messageData);

	RETURN_VOID();
}

SSlua::ArgumentList ComponentSubscribeToMessage(const SSlua::ArgumentList& aArgumentList)
{
	CComponentManager* componentManager = CComponentManager::GetInstancePtr();
	if (!componentManager)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in SubscribeToMessage, component manager not created");
		RETURN_VOID();
	}

	if (!ScriptHelper::AssertArgumentList("SubscribeToMessage", { eSSType::NUMBER, eSSType::NUMBER , eSSType::STRING }, aArgumentList, true))
	{
		RETURN_VOID();
	}

	ComponentId componentID = aArgumentList[0].GetUInt();
	CComponent* component = componentManager->GetComponent(componentID);
	if (!component)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in SubscribeToMessage, component with id %u was NULL", componentID);
		RETURN_VOID();
	}

	SComponentMessageCallback callbackInfo;
	callbackInfo.myFunctionName = aArgumentList[2].GetString();
	callbackInfo.myMaybeEnum = aArgumentList[1].GetInt();

	SComponentMessageData messageData;
	messageData.myComponentMessageCallback = &callbackInfo;
	component->Receive(eComponentMessageType::eAddMessageSubscription, messageData);

	RETURN_VOID();
}

SSlua::ArgumentList ComponentGetParentPosition(const SSlua::ArgumentList& aArgumentList)
{
	CComponentManager* componentManager = CComponentManager::GetInstancePtr();
	if (!componentManager)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetParentPosition, component manager not created");
		return{ ssLuaNumber(0.0), ssLuaNumber(0.0) , ssLuaNumber(0.0) };
	}

	if (!ScriptHelper::AssertArgumentList("GetParentPosition", { eSSType::NUMBER }, aArgumentList, true))
	{
		return{ ssLuaNumber(0.0), ssLuaNumber(0.0) , ssLuaNumber(0.0) };
	}

	ComponentId componentID = aArgumentList[0].GetUInt();
	CComponent* component = componentManager->GetComponent(componentID);
	if (!component)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetParentPosition, component is null");
		return{ ssLuaNumber(0.0), ssLuaNumber(0.0) , ssLuaNumber(0.0) };
	}

	PAPA = component->GetParent();
	if (!parent)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetParentPosition, component's parent is null");
		return{ ssLuaNumber(0.0), ssLuaNumber(0.0) , ssLuaNumber(0.0) };
	}

	CU::Vector3f parentPosition(parent->GetWorldPosition());

	SSlua::ArgumentList position(3);

	position.Add(ssLuaNumber(parentPosition.x));
	position.Add(ssLuaNumber(parentPosition.y));
	position.Add(ssLuaNumber(parentPosition.z));

	return position;
}

SSlua::ArgumentList GetMessageData(const SSlua::ArgumentList& aArgumentList)
{
	SSlua::ArgumentList messageData(1);

	if (!ScriptHelper::AssertArgumentList("GetMessageData", { eSSType::LIGHTUSERDATA, eSSType::STRING }, aArgumentList, true))
	{
		messageData.Add(SSArgument());
		return messageData;
	}

	void* rawData = aArgumentList[0].GetUserData();
	SComponentMessageData* realData = static_cast<SComponentMessageData*>(rawData);
	if (!realData || !realData->myVoidPointer)
	{
		messageData.Add(SSArgument());
		return messageData;
	}

	std::string stringArg = aArgumentList[1].GetString();
	CU::ToLowerCase(stringArg);
	if (stringArg == "number")
	{
		messageData.Add(ssLuaNumber(realData->myInt));
		return messageData;
	}
	else if (stringArg == "string")
	{
		const char* str = realData->myString;
		if (strlen(str) < 100u) // if it is to large, it is probably an error
		{
			messageData.Add(ssLuaString(realData->myString));
			return messageData;
		}
	}
	else if (stringArg == "gameobject")
	{
		messageData.Add(ssLuaNumber(realData->myGameObject->GetId()));
		return messageData;
	}
	else if (stringArg == "component")
	{
		messageData.Add(ssLuaNumber(realData->myComponent->GetId()));
		return messageData;
	}

	messageData.Add(SSArgument());
	return messageData;
}

SSlua::ArgumentList GetMessageType(const SSlua::ArgumentList& aArgumentList)
{
	SSlua::ArgumentList messageType(1);

	if (!ScriptHelper::AssertArgumentList("GetMessageType", { eSSType::STRING }, aArgumentList, true))
	{
		messageType.Add(ssLuaNumber(-1.0));
		return messageType;
	}

	const std::string typeName = aArgumentList[0].GetString();
	int typeInt = ComponentMessage::GetType(typeName);
	if (typeInt == -1)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in GetMessageType, message type name %s could not match any message type", typeName.c_str());
		messageType.Add(ssLuaNumber(-1.0));
		return messageType;
	}

	messageType.Add(ssLuaNumber(typeInt));
	return messageType;
}

SSlua::ArgumentList SpawnParticles(const SSlua::ArgumentList& aArgumentList)
{
	CParticleEffectManager* particleEffectManager = CParticleEffectManager::GetInstance();
	if (!particleEffectManager)
	{
		GLOBAL_LUA_FUNCTION_ERROR("Error in SpawnParticles, particle effect manager was null");
		RETURN_VOID();
	}

	if (!ScriptHelper::AssertArgumentList("SpawnParticles", { eSSType::STRING, eSSType::NUMBER, eSSType::NUMBER, eSSType::NUMBER }, aArgumentList, true))
	{
		RETURN_VOID();
	}

	const std::string particleType = aArgumentList[0].GetString(); // type
	CU::Vector3f spawnPosition(aArgumentList[1].GetFloat(), aArgumentList[2].GetFloat(), aArgumentList[3].GetFloat());

	particleEffectManager->SpawnParticle(particleType, spawnPosition);

	RETURN_VOID();
}

SSlua::ArgumentList ChangeLevel(const SSlua::ArgumentList& aArgumentList)
{
	if (!ScriptHelper::AssertArgumentCount("ChangeLevel", 1, aArgumentList.Size(), true))
	{
		RETURN_VOID();
	}

	CU::CJsonValue levelDocument;
	std::string jsonError = levelDocument.Parse("Json/LevelList.json");
	if (!jsonError.empty())
	{
		DL_MESSAGE_BOX("Failed to parse LevelList.json: %s", jsonError.c_str());
		RETURN_VOID();
	}

	CU::CJsonValue levelArray = levelDocument["levels"];

	int levelIndex = -1;
	if (ScriptHelper::CheckArgumentList("ChangeLevel", { eSSType::STRING }, aArgumentList, true))
	{
		const std::string typedName = aArgumentList.GetFirst().GetString();
		if (typedName.size() == 1 && CU::StringHelper::IsInt(typedName))
		{
			levelIndex = std::atoi(typedName.c_str());
		}
		else
		{
			for (int i = 0; i < levelArray.Size(); ++i)
			{
				if (levelArray[i].GetString() == typedName)
				{
					levelIndex = i;
					break;
				}
			}
			DL_MESSAGE_BOX("Could not find level with name %s", typedName.c_str());
		}
	}
	else if (ScriptHelper::CheckArgumentList("ChangeLevel", { eSSType::NUMBER }, aArgumentList, true))
	{
		levelIndex = aArgumentList.GetFirst().GetInt();
	}
	else
	{
		DL_MESSAGE_BOX("Wrong argument type in ChangeLevel, expected string or number, got: %s", aArgumentList.GetFirst().GetTypeName());
	}
	if (levelIndex < 0 || levelIndex >= levelArray.Size())
	{
		DL_MESSAGE_BOX("Trying to change to level index out of range");
		RETURN_VOID();
	}

	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CChangeLevel(eMessageType::eChangeLevel, levelIndex));
	RETURN_VOID();
}

SSlua::ArgumentList IsPlayer(const SSlua::ArgumentList& aArgumentList)
{
	SSlua::ArgumentList isPlayer(1);
	isPlayer.Add(SSArgument(false));
	
	if (ScriptHelper::CheckArgumentList("IsPlayer", { eSSType::NUMBER }, aArgumentList, true))
	{
		ComponentId id = aArgumentList.GetFirst().GetUInt();
		if (id != NULL_COMPONENT)
		{
			CComponent* maybePlayer = CComponentManager::GetInstance().GetComponent(id);
			if (CPollingStation::GetInstance() && maybePlayer == CPollingStation::GetInstance()->GetPlayerObject())
			{
				isPlayer.GetFirst() = true;
			}
		}
	}

	return isPlayer;
}

SSlua::ArgumentList SetUserData(const SSlua::ArgumentList& aArgumentList)
{
	if (ScriptHelper::CheckArgumentList("SetUserData", { eSSType::NUMBER, eSSType::STRING }, aArgumentList, false))
	{
		ComponentId componentID = aArgumentList.GetFirst().GetUInt();
		if (componentID != NULL_COMPONENT)
		{
			CComponent* component = CComponentManager::GetInstance().GetComponent(componentID);
		}
	}
	else if (ScriptHelper::AssertArgumentList("SetUserData", { eSSType::STRING }, aArgumentList, false))
	{
	}

	RETURN_VOID();
}

SSlua::ArgumentList LuaAssert(const SSlua::ArgumentList& aArgumentList)
{
	std::string errorMessage;
	bool assersionSucceeded = false;
	
	for (const SSArgument& argument : aArgumentList)
	{
		switch (argument.GetType())
		{
		case eSSType::BOOL:
			assersionSucceeded = argument.GetBool();
			break;
		case eSSType::STRING:
			errorMessage += argument.GetString();
			break;
		default:
			errorMessage += argument.AsString();
		}
	}

	if (!assersionSucceeded)
	{
		DL_MESSAGE_BOX("Lua assersion failed: %s", errorMessage.c_str());
	}

	RETURN_VOID();
}
