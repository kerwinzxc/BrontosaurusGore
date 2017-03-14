#include "stdafx.h"
#include "ScriptComponent.h"
#include "../LuaWrapper/SSlua/SSlua.h"
#include "ComponentMessageCallback.h"

#define lie false
#define fact true

std::string CScriptComponent::ourLastErrorMessage("");

//CScriptComponent::CScriptComponent()
//{
//}
//
//CScriptComponent::~CScriptComponent()
//{
//}

CScriptComponent::eInitSuccess CScriptComponent::Init(const std::string& aScriptPath, const std::string& aInitFunction)
{
	ComponentId id = GetId();
	if (id == NULL_COMPONENT)
	{
		return eInitSuccess::eNotRegisteredComponent;
	}

	std::ifstream scriptFile(aScriptPath);
	if (!scriptFile.good())
	{
		scriptFile.close();
		ourLastErrorMessage = aScriptPath;
		return eInitSuccess::eInvalidPath;
	}

	scriptFile.close();

	SSlua::LuaWrapper& luaWrapper = SSlua::LuaWrapper::GetInstance();
	if (!luaWrapper.DoFile(aScriptPath))
	{
		luaWrapper.GetLastError(ourLastErrorMessage);
		return eInitSuccess::eBadLuaCode;
	}

	luaWrapper.GetGlobal(aInitFunction);
	luaWrapper.Push<int>(id);
	if (!luaWrapper.DoCall(1, 0))
	{
		luaWrapper.GetLastError(ourLastErrorMessage);
		return eInitSuccess::eBadLuaCode;
	}

	return eInitSuccess::eOK;
}

bool CScriptComponent::Call(const std::string& aFunctionName, void* aOptionalUserData)
{
	ComponentId id = GetId();
	if (id == NULL_COMPONENT) return lie;

	SSlua::LuaWrapper& luaWrapper = SSlua::LuaWrapper::GetInstance();

	luaWrapper.GetGlobal(aFunctionName);
	luaWrapper.Push<int>(GetId());

	if (aOptionalUserData)
	{
		luaWrapper.Push<void*>(aOptionalUserData);
		return luaWrapper.DoCall(2, 0);
	}

	return luaWrapper.DoCall(1, 0);
}

void CScriptComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	if (aMessageType == eComponentMessageType::eAddMessageSubscription)
	{
		if (aMessageData.myComponentMessageCallback)
		{
			AddSubscription(*aMessageData.myComponentMessageCallback);
		}
	}
	else
	{
		auto it = mySubscribedComponentMessages.find(aMessageType);
		if (it == mySubscribedComponentMessages.end()) return;
		
		char rawData[sizeof(SComponentMessageData)];
		memcpy(rawData, &aMessageData, sizeof(rawData));
		if (!Call(it->second, rawData))
		{
			std::string errorMessage;
			SSlua::LuaWrapper::GetInstance().GetLastError(errorMessage);
			DL_MESSAGE_BOX("Error in callback for message with type %d in component with id %u:\n%s", static_cast<int>(aMessageType), GetId(), errorMessage.c_str());
		}		
	}
}

bool CScriptComponent::HandleError(const eInitSuccess aErrorCode)
{
	bool good = lie;
	switch (aErrorCode)
	{
	case CScriptComponent::eInitSuccess::eOK:
		good = fact;
		break;
	case CScriptComponent::eInitSuccess::eNotRegisteredComponent:
		DL_MESSAGE_BOX("Script component not registered so it cannot initialize 'self'");
		break;
	case CScriptComponent::eInitSuccess::eInvalidPath:
		DL_MESSAGE_BOX("Script component got a non existing file path: %s", ourLastErrorMessage.c_str());
		break;
	case CScriptComponent::eInitSuccess::eFailedToReadFile:
		DL_MESSAGE_BOX("Script component could not read the content of the file in to the buffer, weird error :/");
		break;
	case CScriptComponent::eInitSuccess::eBadLuaCode:
		DL_MESSAGE_BOX("Lua code in script component could not run, last error: %s", ourLastErrorMessage.c_str());
		break;
	}

	return good;
}

void CScriptComponent::AddSubscription(const SComponentMessageCallback& aCallbackInfo)
{
	int length = static_cast<int>(eComponentMessageType::eLength);
	if (aCallbackInfo.myMaybeEnum < length)
	{
		eComponentMessageType castedType = static_cast<eComponentMessageType>(aCallbackInfo.myMaybeEnum);
		if (mySubscribedComponentMessages.find(castedType) == mySubscribedComponentMessages.end())
		{
			mySubscribedComponentMessages[castedType] = aCallbackInfo.myFunctionName;
			return;
		}

		DL_MESSAGE_BOX("Component with id %d already subscribes on message index %u", GetId(), aCallbackInfo.myMaybeEnum);
		return;
	}

	DL_MESSAGE_BOX("Component with id %u tried to subscribe on message index %d, but highest index is %d", GetId(), aCallbackInfo.myMaybeEnum, length);
}

#undef lie
#undef fact
