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
#include "WindowsHelper.h"
#include "Engine.h"
#include "DXFramework.h"
#include "WindowsWindow.h"
//#include "ThreadedPostmaster/Postmaster.h"


CTempLobbyState::CTempLobbyState(StateStack & aStateStack) : State(aStateStack, eInputMessengerType::eTempLobbyState)
, myLobbyState(eLobbyState::eEnterIpAndName)
, myCurrentLine(0)
, myTextInputSelected(false)
, myBlinkeyTimer(0)
, myBlinkeyState(false)
, myIsPlayer(false)
{
}


CTempLobbyState::~CTempLobbyState()
{
}

bool CTempLobbyState::IsSelectable(const unsigned char aCurrentLine)
{
	if (myTextINstance.GetTextLines()[aCurrentLine] == L"")
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
			} while (IsSelectable(myCurrentLine) == false && !myTextINstance.Empty());
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
			} while (IsSelectable(myCurrentLine) == false && !myTextINstance.Empty());
			break;
		case 7:
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
			myStateStack.PushState(new CLoadState(myStateStack, myCurrentLine - 4));
			CNetworkMessage_LoadLevel* netowrkMessageMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_LoadLevel>("__All_But_Me");
			netowrkMessageMessage->myLevelIndex = myCurrentLine - 4;
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(netowrkMessageMessage));
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
	else if (myCurrentLine == 6)
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
		const CU::GrowingArray<std::wstring> names({ L"Ralle", L"Adam", L"Fröken", L"Stefan", L"Ralle", L"Adam", L"Fröken", L"Stefan" , L"Ralle", L"Adam", L"Fröken", L"Stefan", L"Bjarne Stroustrup" });
		const CU::GrowingArray<std::wstring>::size_type randIndex = rand() % names.Size();

		myName = names.At(randIndex);
	}

	if (myIP.empty())
	{

		myIP = L"127.0.0.1";
	}

	if(myIP == L"127.0.0.1" && TShared_NetworkWrapper::CheckPortOpen(SERVER_PORT) == true)
	{
		std::string processName = "TServer_Applictaion_x64_";

#ifdef _DEBUG
		processName += "Debug";
#elif defined(RETAIL)
		processName = "HighDoomServer";
#elif defined(RELEASE)
		processName += "Release";
#endif

		processName += ".exe";
		if (!WindowsHelper::StartProgram(processName))
		{
			DL_MESSAGE_BOX("Failed to start server, failed to build TServer_Application?\n%s", processName.c_str());
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		WindowsHelper::SetFocus(CEngine::GetInstance()->GetWindow()->GetHWND());
	}

	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CConectMessage(CU::StringHelper::WStringToString(myName), CU::StringHelper::WStringToString(myIP)));
}

void CTempLobbyState::HandleKeyPress(const CU::SInputMessage& aInputMessage)
{
	if (myLobbyState == eLobbyState::eConecting)
	{
		return;
	}

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
			} while (IsSelectable(myCurrentLine) == false && !myTextINstance.Empty());
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
			} while (IsSelectable(myCurrentLine) == false && !myTextINstance.Empty());
		}
		break;
	case CU::eKeys::RETURN:
		Select();
		break;
	case CU::eKeys::BACK:
		Back();
		break;
	case CU::eKeys::ESCAPE:
		SetStateStatus(eStateStatus::ePop);
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
	GetIPAddress();
}

void CTempLobbyState::LobbyMenu()
{
	std::wstring string;
	myTextINstance.SetText(L"");

	std::wstring ipAddressStr = L"Your ip address is : " + myThisComputersIP;
	if (myThisComputersIP.empty())
	{
		ipAddressStr = L"We don't know your ip, google it";
	}

	myTextINstance.SetTextLine(0, L"# Welcome! Provide the ip adress (Optional) and a name (Optional). " + ipAddressStr);
	myTextINstance.SetTextLine(1, L"# You navigate with the arrow keyes and when you are done go down to \"done\" and press enter");
	myTextINstance.SetTextLine(2, L"# If you are playing with other players and wan't to host you don't have to enter an ip adress");
	myTextINstance.SetTextLine(3, L"# If you are playing eith other players and wan't to join a host you should enter their IP");
	myTextINstance.SetTextLine(4, L"# And if you don't enter a name one will be provided for you");



	const int IProw = 5;
	(string = L"IP: ") += myIP.c_str();
	if (myCurrentLine == IProw && myBlinkeyState == true)
	{
		string += L"I";
	}
	myTextINstance.SetTextLine(IProw, string);

	const int nameLine = 6;
	(string = L"Name: ") += myName.c_str();
	if (myCurrentLine == nameLine && myBlinkeyState == true)
	{
		string += L"I";
	}
	myTextINstance.SetTextLine(nameLine, string);


	myTextINstance.SetTextLine(7, L"Done");


	if (myCurrentLine < myTextINstance.GetTextLines().Size() && myCurrentLine >= 0)
	{
		myTextINstance.SetTextLine(myCurrentLine, myTextINstance.GetTextLines()[myCurrentLine] + L" <-");
	}
}

void CTempLobbyState::LevelSelect()
{

	myTextINstance.SetText(L"");

	std::wstring string;
	string = L"# Welcome player: ";
	string += myName;
	string += L" you are conected to the IP: ";
	string += myIP;

	myTextINstance.SetTextLine(0, string.c_str());


	if (myIsPlayer == true)
	{
		std::wstring ipStr = L"# Your ip address is " + myThisComputersIP;
		if (myThisComputersIP.empty())
		{
			ipStr = L"# We don't know your ip address, google it :(";
		}

		myTextINstance.SetTextLine(0, L"# Other players can only join when you are on this screen");
		myTextINstance.SetTextLine(1, std::move(ipStr));
		myTextINstance.SetTextLine(2, L"# Please Select level");

		CU::CJsonValue levelsFile;

		std::string errorString = levelsFile.Parse("Json/LevelList.json");
		if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

		CU::CJsonValue levelsArray = levelsFile.at("levels");

		for (int i = 0; i < levelsArray.Size(); ++i)
		{
			myTextINstance.SetTextLine(4 + i, CU::StringToWString(levelsArray[i].GetString()));
		}

		if (myCurrentLine < myTextINstance.GetTextLines().Size() && myCurrentLine >= 0)
		{
			myTextINstance.SetTextLine(myCurrentLine, myTextINstance.GetTextLines()[myCurrentLine] + L" <-");
		}
	}
	else
	{
		myTextINstance.SetTextLine(1, L"# Waiting for the host to select a level");
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
		myTextINstance.SetText(L"");

		std::wstring string = L"#Conecting";
		for (int i = 0; i < static_cast<int>(myBlinkeyTimer.GetSeconds()) % 4; ++i)
		{
			string += L".";
		}

		myTextINstance.SetTextLine(0, string.c_str());
	}
	break;
	default: break;
	}

	myCurrentLine = CLAMP(myCurrentLine, 0, myTextINstance.GetTextLines().Size() - 1);
	if (!(myLobbyState == eLobbyState::eConecting || (myLobbyState == eLobbyState::eSelectLevel && myIsPlayer == false)))
	{
		while (IsSelectable(myCurrentLine) == false && !myTextINstance.Empty())
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

	return myStatus;
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

#include <algorithm>
#include <Iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

void CTempLobbyState::GetIPAddress()
{
	PIP_ADAPTER_ADDRESSES addresses = nullptr;
	ULONG bufferSize = sizeof(IP_ADAPTER_ADDRESSES);
	ULONG result = ERROR_SUCCESS;
	bool errorOccured = false;

	do 
	{
		addresses = (PIP_ADAPTER_ADDRESSES)malloc(bufferSize);
		if (addresses == nullptr)
		{
			break;
		}

		result = GetAdaptersAddresses(AF_INET,
			GAA_FLAG_SKIP_ANYCAST |
			GAA_FLAG_SKIP_MULTICAST |
			GAA_FLAG_SKIP_DNS_SERVER |
			GAA_FLAG_SKIP_FRIENDLY_NAME, nullptr, addresses, &bufferSize);

		if (result == ERROR_BUFFER_OVERFLOW)
		{
			free(addresses);
			addresses = nullptr;
		}
	}
	while (result == ERROR_BUFFER_OVERFLOW);
	//addresses->FirstUnicastAddress->Address.lpSockaddr->sa_family;

	do 
	{
		if (result == ERROR_SUCCESS)
		{
			if (!addresses || !addresses->FirstUnicastAddress || !addresses->FirstUnicastAddress->Address.lpSockaddr)
			{
				errorOccured = true;
				break;
			}

			SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(addresses->FirstUnicastAddress->Address.lpSockaddr);

			char strBuffer[INET_ADDRSTRLEN] = {};
			if (inet_ntop(AF_INET, &(ipv4->sin_addr), strBuffer, INET_ADDRSTRLEN) == nullptr)
			{
				errorOccured = true;
				//WSAGetLastError();
				break;
			}

			myThisComputersIP = std::wstring(std::begin(strBuffer), std::end(strBuffer));
		}
	} while (false);
	
	if (errorOccured)
	{
		myThisComputersIP.clear();
	}
}
