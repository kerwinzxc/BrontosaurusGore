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
#include "ThreadedPostmaster/LoadLevelMessage.h"
#include "CommonUtilities.h"
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
		case 5:
			//myLobbyState = eLobbyState::eEnterIp;
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
			break;
		case 6:
			//myLobbyState = eLobbyState::eEnterName;
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
			break;
		case 8:
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
			if (myIsPlayer == true)
			{
				myStateStack.PushState(new CLoadState(myStateStack, myCurrentLine - 3));
				CNetworkMessage_LoadLevel* netowrkMessageMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_LoadLevel>("__All_But_Me");
				netowrkMessageMessage->myLevelIndex = myCurrentLine - 3;
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(netowrkMessageMessage));
			}
		}
		break;
	default: break;
	}
}

void CTempLobbyState::Back()
{
	if (myCurrentLine == 5)
	{
		if (myIP.empty() == false)
		{
			myIP.erase(myIP.end() - 1);
		}
	}
	else if	(myCurrentLine == 6)
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
		const CU::GrowingArray<std::string> names({"Ralle", "Adam", "Fröken", "Stefan", "Ralle", "Adam", "Fröken", "Stefan" , "Ralle", "Adam", "Fröken", "Stefan", "Bjarne Stroustrup"});
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
	std::string string;
	myTextINstance.SetText("");

	myTextINstance.SetTextLine(0, "#This is the first page of the lobby! Here you will provide the ip adress and a name");
	myTextINstance.SetTextLine(1, "# You navigate with the arrow keyes and when you are done go down to \"done\" and press enter");
	myTextINstance.SetTextLine(2, "# If you are playing with other players and wan't to host you don't have to enter an ip adress");
	myTextINstance.SetTextLine(3, "# If you are playing eith other players and wan't to join a host you should enter their IP");
	myTextINstance.SetTextLine(4, "# And if you don't enter a name one will be provided for you");

	

	const int IProw = 5;
	(string = "IP: ") += myIP.c_str();
	if (myCurrentLine == IProw && myBlinkeyState == true)
	{
		string += "I";
	}
	myTextINstance.SetTextLine(IProw, string);

	const int nameLine = 6;
	(string = "Name: ") += myName.c_str();
	if (myCurrentLine == nameLine && myBlinkeyState == true)
	{
		string += "I";
	}
	myTextINstance.SetTextLine(nameLine, string);


	myTextINstance.SetTextLine(8, "Done");
	

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
		myTextINstance.SetTextLine(0, "# Other players can only join when you are on this screen");
		myTextINstance.SetTextLine(1, "# DO NOT SELECT A LEVEL UNTIL ALL OTHER PLAYERS HAVE JONED");
		myTextINstance.SetTextLine(2, "# Please Select level");
		
		CU::CJsonValue levelsFile;

		std::string errorString = levelsFile.Parse("Json/LevelList.json");
		if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

		CU::CJsonValue levelsArray = levelsFile.at("levels");

		for (int i = 0; i < levelsArray.Size(); ++i)
		{
			myTextINstance.SetTextLine(4 + i, levelsArray[i].GetString().c_str());
		}

		if (myCurrentLine < myTextINstance.GetTextLines().Size() && myCurrentLine >= 0)
		{
			myTextINstance.SetTextLine(myCurrentLine, myTextINstance.GetTextLines()[myCurrentLine] + " <-");
		}
	}
	else
	{
		myTextINstance.SetTextLine(1, "# Waiting for the host to select a level");
	}
}

eStateStatus CTempLobbyState::Update(const CU::Time& aDeltaTime)
{
	myCurrentLine = CLAMP(myCurrentLine, 0, myTextINstance.GetTextLines().Size() - 1);

	myBlinkeyTimer += aDeltaTime;
	if (static_cast<int>(myBlinkeyTimer.GetSeconds()) % 2 == 0)
	{
		myBlinkeyState = true;
	}
	else
	{
		myBlinkeyState = false;
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

			std::string string = "#Conecting";
			for (int i = 0; i < static_cast<int>(myBlinkeyTimer.GetSeconds()) % 4; ++i)
			{
				string += ".";
			}

			myTextINstance.SetTextLine(0, string.c_str());
		}
		break;
	default: break;
	}

	myCurrentLine = CLAMP(myCurrentLine, 0, myTextINstance.GetTextLines().Size() - 1);
	if (myLobbyState != eLobbyState::eConecting)
	{
		while (IsSelectable(myCurrentLine) == false)
		{
			if (myCurrentLine < myTextINstance.GetTextLines().Size() - 1)
			{
				myCurrentLine += 1;
			}
			else
			{
				myCurrentLine = 0;
			}
		}
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
	if (myLobbyState == eLobbyState::eEnterIpAndName && myCurrentLine == 5)
	{
		const char inputChar = aCharPressed.GetKey();

		if (ipCheck.find(inputChar) != std::string::npos)
		{
			myIP += inputChar;
		}
	}
	else if (myLobbyState == eLobbyState::eEnterIpAndName && myCurrentLine == 6)
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

eMessageReturn CTempLobbyState::DoEvent(const CLoadLevelMessage& aLoadLevelMessage)
{
	myStateStack.PushState(new CLoadState(myStateStack, aLoadLevelMessage.myLevelIndex));
	return eMessageReturn::eContinue;
}
