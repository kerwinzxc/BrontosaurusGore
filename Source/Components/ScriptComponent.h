#pragma once
#include "Component.h"

struct SComponentMessageCallback;

class CScriptComponent : public CComponent
{
public:
	enum class eInitSuccess
	{
		eOK,
		eNotRegisteredComponent,
		eInvalidPath,
		eFailedToReadFile,
		eBadLuaCode
	};

	CScriptComponent() = default;
	~CScriptComponent() = default;

	eInitSuccess Init(const std::string& aScriptPath, const std::string& aInitFunction, const std::map<std::string, std::string>& aDataMap);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Call(const std::string& aFunctionName, void* aOptionalUserData = nullptr);

	static bool HandleError(const eInitSuccess aErrorCode);

private:
	void AddSubscription(const SComponentMessageCallback& aCallbackInfo);

	std::map<eComponentMessageType, std::string> mySubscribedComponentMessages;

	static std::string ourLastErrorMessage;
};

