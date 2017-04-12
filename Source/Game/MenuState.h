#pragma once
#include "StateStack/State.h"
#include "GUI/MenuManager.h"
#include "JsonValue.h"

class CMenuState :public State
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

private:
	static eAlignment LoadAlignment(const CU::CJsonValue& aJsonValue);
	void LoadElement(const CU::CJsonValue& aJsonValue, const std::string& aFolderpath);
	void MenuLoad(const std::string& aFile);

	static void ExitGame(std::string notUsed);
	void PushTempLobby(std::string notUsed) const;

	bool myShowStateBelow;
	CSpriteInstance* myPointerSprite;

	CMenuManager myManager;
};

inline bool CMenuState::GetLetThroughRender() const
{
	return myShowStateBelow;
}

inline bool CMenuState::GetLetThroughUpdate() const
{
	return false;
}

