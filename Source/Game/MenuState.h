#pragma once
#include "../StateStack/State.h"
#include "../GUI/MenuManager.h"
#include "JsonValue.h"
#include "../ThreadedPostmaster/Subscriber.h"

struct STextInput
{
	STextInput(): myInputIsValid(true), myTextInstance(nullptr)
	{
	}

	bool myInputIsValid;
	CTextInstance* myTextInstance;
};

class CMenuState :public State, Postmaster::ISubscriber
{
public:

	CMenuState(StateStack& aStateStack, std::string aFile);
	~CMenuState();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;

	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	bool GetLetThroughRender() const override;
	bool GetLetThroughUpdate() const override;

	CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;

	eMessageReturn DoEvent(const KeyCharPressed& aCharPressed) override;
	eMessageReturn DoEvent(const CConectedMessage& aCharPressed) override;
	eMessageReturn DoEvent(const CLoadLevelMessage& aLoadLevelMessage) override;

private:
	static eAlignment LoadAlignment(const CU::CJsonValue& aJsonValue);
	void LoadElement(const CU::CJsonValue& aJsonValue, const std::string& aFolderpath);
	void MenuLoad(const std::string& aFile);
	bool PushMenu(std::string aMenu);

	static bool ExitGame(std::string notUsed);
	bool PushTempLobby(std::string);
	bool PopMenues(std::string aNumberOfMenues);
	bool PushLevel(std::string aLevelIndexString);
	static bool StartServer(std::string notUsed);
	bool ConnectLocal(std::string anIp);
	bool SetCurrentTextInput(std::string aTexINputIndex);
	bool CheckIp(std::string aTextInput);
	bool SetName(std::string aTextInput);
	bool SetIp(std::string aTextInput);
	bool Conect(std::string notusese);
	bool SwapLevel(std::string levelndex);

	void GetIPAddress();
	CU::GrowingArray<STextInput> myTextInputs;
	int myCurrentTextInput;

	bool myShowStateBelow;
	CSpriteInstance* myPointerSprite;

	CMenuManager myManager;
	bool myIsInFocus;
	std::wstring myThisComputersIP;
	static char ourMenuesToPop;

	bool myBlinkeyBool;
	float myBlinkeyTimer;

	std::string myName;
	std::string myIp;
};

inline bool CMenuState::GetLetThroughRender() const
{
	return myShowStateBelow;
}

inline bool CMenuState::GetLetThroughUpdate() const
{
	return false;
}

