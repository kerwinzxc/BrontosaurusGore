#pragma once
#include "StateStack/State.h"
#include "TextInstance.h"
//#include "ThreadedPostmaster/Subscriber.h"
#include "PostMaster/MessageType.h"
#include "ThreadedPostmaster/Subscriber.h"

enum class eLobbyState
{
	eEnterIpAndName,
	eSelectLevel,
	eEnterIp,
	eEnterName,
	eConecting,
};

class CTempLobbyState : public State,  Postmaster::ISubscriber
{
public:
	CTempLobbyState(StateStack& aStateStack);
	~CTempLobbyState();

	bool IsSelectable(const unsigned char aCurrentLine);

	void Select();
	void Back();
	void Conect();

	void HandleKeyPress(const CU::SInputMessage& aInputMessage);
	void HandleKeyRelease(const CU::SInputMessage& aInputMessage);
	CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;

	void Init() override;
	void LobbyMenu();
	void LevelSelect();
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;

	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	bool GetLetThroughRender() const override;
	bool GetLetThroughUpdate() const override;

	eMessageReturn DoEvent(const KeyCharPressed& aCharPressed) override;
	eMessageReturn DoEvent(const CConectedMessage& aCharPressed) override;
	//eMessageReturn DoEvent(const  )

private:
	CTextInstance myTextINstance;
	
	eLobbyState myLobbyState;
	eStateStatus myStateStatus;

	unsigned char myCurrentLine;

	std::string myIP;
	std::string myName;
	CU::Time myBlinkeyTimer;

	bool myTextInputSelected;
	bool myBlinkeyState;
	bool myIsPlayer;
};

