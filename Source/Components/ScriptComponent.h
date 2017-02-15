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
		eBadLuaCode,
		eNoFunctionTable
	};

	CScriptComponent();
	~CScriptComponent();

	eInitSuccess Init(const std::string& aScriptPath, const std::string& aInitFunction);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Call(const std::string& aFunctionName, void* aOptionalUserData = nullptr);

	static bool HandleError(const eInitSuccess aErrorCode);

private:
	void AddSubscription(const SComponentMessageCallback& aCallbackInfo);
	void Destroy() override;

	std::map<eComponentMessageType, std::string> mySubscribedComponentMessages;

	static std::string ourLastErrorMessage;
};

