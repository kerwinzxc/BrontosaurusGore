#include "stdafx.h"
#include "TempLobbyState.h"


CTempLobbyState::CTempLobbyState(StateStack & aStateStack): State(aStateStack, eInputMessengerType::eTempLobbyState), myLobbyState(eLobbyState::eEnterIpAndName), myCurrentLine(0)
{
}


CTempLobbyState::~CTempLobbyState()
{
}

void CTempLobbyState::Init()
{
	myTextINstance.Init();
	myTextINstance.SetColor({ 0.f,1.f,1.f,1.f });
}

eStateStatus CTempLobbyState::Update(const CU::Time& aDeltaTime)
{
	switch (myLobbyState)
	{
	case eLobbyState::eEnterIpAndName:
		{
			myTextINstance.SetTextLine(0, "IP :");
			myTextINstance.SetTextLine(1,"Name");
			myTextINstance.SetTextLine(4, "Done(If ip and name is left empty they will be filled in for you with ip being set to local)");

			if (myCurrentLine < myTextINstance.GetTextLines().Size() && myCurrentLine >= 0)
			{
				myTextINstance.SetTextLine(myCurrentLine, myTextINstance.GetTextLines()[myCurrentLine] + " <-");
			}
		}
		break;
	case eLobbyState::eSelectLevel:
		break;
	default: break;
	}

	return eStateStatus::eKeep;
}
