#include "stdafx.h"
#include "TempLobbyState.h"
#include "InputMessage.h"
#include "EInputReturn.h"
#include "EKeyboardKeys.h"
#include "ThreadedPostmaster/Postmaster.h"
#include "PostMaster/KeyCharPressed.h"
#include "ThreadedPostmaster/ConetctMessage.h"
#include "ThreadedPostmaster/ConectedMessage.h"
#include "TShared/TShared_NetworkWrapper.h"
#include "JsonValue.h"
#include "StateStack/StateStack.h"
#include "PlayState.h"
#include "LoadState.h"
#include "TClient/ClientMessageManager.h"
#include "TShared/NetworkMessage_LoadLevel.h"
#include "ThreadedPostmaster/SendNetowrkMessageMessage.h"
//#include "ThreadedPostmaster/Postmaster.h"


CTempLobbyState::CTempLobbyState(StateStack & aStateStack) : State(aStateStack, eInputMessengerType::eTempLobbyState)
, myLobbyState(eLobbyState::eEnterIpAndName)
, myCurrentLine(0)
, myTextInputSelected(false)
, myBlinkeyTimer(0)
, myBlinkeyState(false)
, myIsPlayer(false)
, myStateStatus(eStateStatus::eKeep)
{
}


CTempLobbyState::~CTempLobbyState()
{
}

bool CTempLobbyState::IsSelectable(const unsigned char aCurrentLine)
{
	if (myTextINstance.GetTextLines()[aCurrentLine] == "")
	{
		return false;
	}

	if (myTextINstance.GetTextLines()[aCurrentLine].at(0) == '#')
	{
		return false;
	}
	return true;
}

void CTempLobbyState::Select()
{
	switch (myLobbyState)
	{
	case eLobbyState::eEnterIpAndName:
		switch (myCurrentLine)
		{
		case 0:
			//myLobbyState = eLobbyState::eEnterIp;
			break;
		case 1:
			//myLobbyState = eLobbyState::eEnterName;
			break;
		case 3:
			Conect();
			break;
		default: break;
		}
		break;
	case eLobbyState::eEnterIp:
	case eLobbyState::eEnterName:
		myLobbyState = eLobbyState::eEnterIpAndName;
		break;
	case eLobbyState::eSelectLevel:
		{
			myStateStack.PushState(new CLoadState(myStateStack, myCurrentLine - 2));
			CNetworkMessage_LoadLevel* netowrkMessageMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_LoadLevel>("__Server");
			netowrkMessageMessage->myLevelIndex = myCurrentLine - 2;
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(netowrkMessageMessage));
		}
		break;
	default: break;
	}
}

void CTempLobbyState::Back()
{
	if (myCurrentLine == 0)
	{
		if (myIP.empty() == false)
		{
			myIP.erase(myIP.end() - 1);
		}
	}
	else if	(myCurrentLine == 1)
	{
		if (myName.empty() == false)
		{
			myName.erase(myName.end() - 1);
		}
	}
}

void CTempLobbyState::Conect()
{
	myLobbyState = eLobbyState::eConecting;

	if (myName.empty())
	{
		const CU::GrowingArray<std::string> names({"Ralle", "Adam", "Fröken", "Stefan"});
		const CU::GrowingArray<std::string>::size_type randIndex = rand() % names.Size();

		myName = names.At(randIndex);
	}

	if (myIP.empty())
	{
		myIP = "127.0.0.1";
	}

	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CConectMessage(myName, myIP));
}

void CTempLobbyState::HandleKeyPress(const CU::SInputMessage& aInputMessage)
{
	switch (aInputMessage.myKey)
	{
	case CU::eKeys::UP:
		if (myTextInputSelected == false)
		{
			do
			{
				if (myCurrentLine != 0)
				{
					myCurrentLine -= 1;
				}
				else
				{
					myCurrentLine = myTextINstance.GetTextLines().Size() - 1;
				}
			} while (IsSelectable(myCurrentLine) == false);
		}
		break;
	case CU::eKeys::DOWN:
		if (myTextInputSelected == false)
		{
			do
			{
				if (myCurrentLine < myTextINstance.GetTextLines().Size() - 1)
				{
					myCurrentLine += 1;
				}
				else
				{
					myCurrentLine = 0;
				}
			} while (IsSelectable(myCurrentLine) == false);
		}
		break;
	case CU::eKeys::RETURN:
		Select();
		break;
	case CU::eKeys::BACK:
		Back();
		break;
	case CU::eKeys::ESCAPE:
		myStateStatus = eStateStatus::ePop;
		break;
	default: break;
	}
}

void CTempLobbyState::HandleKeyRelease(const CU::SInputMessage& aInputMessage)
{

}

CU::eInputReturn CTempLobbyState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	switch (aInputMessage.myType)
	{
	case CU::eInputType::eMouseMoved: break;
	case CU::eInputType::eMousePressed: break;
	case CU::eInputType::eMouseReleased: break;
	case CU::eInputType::eScrollWheelChanged: break;
	case CU::eInputType::eKeyboardPressed:
		HandleKeyPress(aInputMessage);
		break;
	case CU::eInputType::eKeyboardReleased:
		HandleKeyRelease(aInputMessage);
		break;
	default: break;
	}
	return CU::eInputReturn::ePassOn;
}

void CTempLobbyState::Init()
{
	myTextINstance.Init();
	myTextINstance.SetColor({ 1.f,1.f,0.f,1.f });
	myTextINstance.SetPosition({ 0.3f,0.3f });
}

void CTempLobbyState::LobbyMenu()
{
	CU::DynamicString string;
	myTextINstance.SetText("");

	(string = "IP: ") += myIP.c_str();
	if (myCurrentLine == 0 && myBlinkeyState == true)
	{
		string += "I";
	}
	myTextINstance.SetTextLine(0, string);

	(string = "Name: ") += myName.c_str();
	if (myCurrentLine == 1 && myBlinkeyState == true)
	{
		string += "I";
	}
	myTextINstance.SetTextLine(1, string);


	myTextINstance.SetTextLine(3, "Done");
	myTextINstance.SetTextLine(5, "#if ip and name is left empty it will be filled in for you");
	myTextINstance.SetTextLine(6, "#use arrow keys to navigate and press enter to select");
	myTextINstance.SetTextLine(7, "# lines starting with \"#\" is info and not selectable");

	if (myCurrentLine < myTextINstance.GetTextLines().Size() && myCurrentLine >= 0)
	{
		myTextINstance.SetTextLine(myCurrentLine, myTextINstance.GetTextLines()[myCurrentLine] + " <-");
	}
}

void CTempLobbyState::LevelSelect()
{

	myTextINstance.SetText("");

	std::string string;
	string = "# Welcome player: ";
	string += myName;
	string += " you are conected to the IP: ";
	string += myIP;

	myTextINstance.SetTextLine(0, string.c_str());


	if (myIsPlayer == true)
	{
		myTextINstance.SetTextLine(1, "# Please Select level");
		
		CU::CJsonValue levelsFile;

		std::string errorString = levelsFile.Parse("Json/LevelList.json");
		if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

		CU::CJsonValue levelsArray = levelsFile.at("levels");

		for (int i = 0; i < levelsArray.Size(); ++i)
		{
			myTextINstance.SetTextLine(2 + i, levelsArray[i].GetString().c_str());
		}

		if (myCurrentLine < myTextINstance.GetTextLines().Size() && myCurrentLine >= 0)
		{
			myTextINstance.SetTextLine(myCurrentLine, myTextINstance.GetTextLines()[myCurrentLine] + " <-");
		}
	}
	else
	{
		myTextINstance.SetTextLine(1, "# Waiting for other player to select a level");
	}
}

eStateStatus CTempLobbyState::Update(const CU::Time& aDeltaTime)
{
	myBlinkeyTimer += aDeltaTime;
	if (static_cast<int>(myBlinkeyTimer.GetSeconds()) % 2 == 0)
	{
		myBlinkeyState = !myBlinkeyState;
	}

	switch (myLobbyState)
	{
	case eLobbyState::eEnterIp:
	case eLobbyState::eEnterName:
	case eLobbyState::eEnterIpAndName:
	{
		LobbyMenu();
	}
	break;
	case eLobbyState::eSelectLevel:
		LevelSelect();
		break;
	case eLobbyState::eConecting:
		{
			myTextINstance.SetText("");

			std::string string = "Conecting";
			for (int i = 0; i < static_cast<int>(myBlinkeyTimer.GetSeconds()) % 4; ++i)
			{
				string += ".";
			}

			myTextINstance.SetTextLine(0, string.c_str());
		}
		break;
	default: break;
	}

	return myStateStatus;
}

void CTempLobbyState::Render()
{
	myTextINstance.Render();
}

void CTempLobbyState::OnEnter(const bool aLetThroughRender)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eCharPressed);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
}

void CTempLobbyState::OnExit(const bool aLetThroughRender)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

bool CTempLobbyState::GetLetThroughRender() const
{
	return false;
}

bool CTempLobbyState::GetLetThroughUpdate() const
{
	return false;
}

eMessageReturn CTempLobbyState::DoEvent(const KeyCharPressed& aCharPressed)
{
	const std::string ipCheck = "1234567890.";
	if (myLobbyState == eLobbyState::eEnterIpAndName && myCurrentLine == 0)
	{
		const char inputChar = aCharPressed.GetKey();

		if (ipCheck.find(inputChar) != std::string::npos)
		{
			myIP += inputChar;
		}
	}
	else if (myLobbyState == eLobbyState::eEnterIpAndName && myCurrentLine == 1)
	{
		const char inputChar = aCharPressed.GetKey();

		if (inputChar != 0x08 && inputChar != 0x0D)
		{
			myName += inputChar;
		}
	}
	return eMessageReturn::eStop;
}

eMessageReturn CTempLobbyState::DoEvent(const CConectedMessage& aCharPressed)
{
	myLobbyState = eLobbyState::eSelectLevel;
	myIsPlayer = aCharPressed.myID == ID_FREE;
	
	myCurrentLine = 2;

	return eMessageReturn::eContinue;
}
