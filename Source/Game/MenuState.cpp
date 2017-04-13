#include "stdafx.h"
#include "MenuState.h"
#include "InputMessage.h"
#include "EInputReturn.h"
#include "JsonValue.h"
#include "EMouseButtons.h"
#include "EKeyboardKeys.h"
#include "StateStack/StateStack.h"
#include "PostMaster/QuitGame.h"
#include "ThreadedPostmaster/Postmaster.h"
#include "TempLobbyState.h"
#include "RenderMessages.h"
#include "Engine.h"
#include "Renderer.h"
#include <Shlwapi.h>
#include "PlayState.h"
#include "LoadState.h"
#include "WindowsHelper.h"
#include "WindowsWindow.h"
#include "ThreadedPostmaster/ConetctMessage.h"
#include "TShared/NetworkMessage_LoadLevel.h"
#include "TClient/ClientMessageManager.h"
#include "ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "PostMaster/KeyCharPressed.h"
#include "ThreadedPostmaster/LoadLevelMessage.h"

char CMenuState::ourMenuesToPop = 0;

CMenuState::CMenuState(StateStack& aStateStack, std::string aFile) : State(aStateStack, eInputMessengerType::eMainMenu), myTextInputs(2), myCurrentTextInput(-1), myShowStateBelow(false), myPointerSprite(nullptr), myIsInFocus(false), myBlinkeyBool(true), myBlinkeyTimer(0)
{
	std::function<bool(std::string)> temp = [this](std::string string)-> bool { return PushTempLobby(string); };

	myManager.AddAction("ExitGame", bind(&CMenuState::ExitGame, std::placeholders::_1));
	myManager.AddAction("PushTempLobby", [this](std::string string)-> bool { return PushTempLobby(string); });
	myManager.AddAction("PushMenu", [this](std::string string)-> bool { return PushMenu(string); });
	myManager.AddAction("PopMenues", [this](std::string string)-> bool { return PopMenues(string); });
	myManager.AddAction("PushLevel", [this](std::string string)-> bool { return PushLevel(string); });
	myManager.AddAction("StartServer", [this](std::string string)-> bool { return StartServer(string); });
	myManager.AddAction("ConectLocal", [this](std::string string)-> bool { return ConnectLocal(string); });
	myManager.AddAction("SelectTextInput", [this](std::string string)-> bool { return SetCurrentTextInput(string); });
	myManager.AddAction("CheckIp", [this](std::string string)-> bool { return CheckIp(string); });
	myManager.AddAction("SetName", [this](std::string string)-> bool { return SetName(string); });
	myManager.AddAction("Conect", [this](std::string string)-> bool { return Conect(string); });
	myManager.AddAction("SetIp", [this](std::string string)-> bool { return SetIp(string); });

	MenuLoad(aFile);
}

CMenuState::~CMenuState()
{
}

void CMenuState::Init()
{
}

eStateStatus CMenuState::Update(const CU::Time& aDeltaTime)
{
	myManager.Update(aDeltaTime);

	myBlinkeyTimer += aDeltaTime.GetSeconds();

	if (myBlinkeyTimer >= 1)
	{
		myBlinkeyBool = !myBlinkeyBool;
		myBlinkeyTimer = 0;
	}

	if (ourMenuesToPop > 0)
	{
		ourMenuesToPop -= 1;
		return eStateStatus::ePop;
	}
	else
	{
		return eStateStatus::eKeep;
	}
}

void CMenuState::Render()
{
	std::wstring oldString;
	std::wstring otherOldString;
	int otherOldStringIndex = -1;

	for (int i = 0; i < myTextInputs.Size(); ++i)
	{
		if (myTextInputs[i].myInputIsValid == false)
		{
			otherOldString = myTextInputs[i].myTextInstance->GetTextLines()[0];
			otherOldStringIndex = i;
			myTextInputs[i].myTextInstance->SetTextLine(0, L"Incorect input");
		}
	}

	if (myCurrentTextInput > -1 && myBlinkeyBool == true)
	{
		CTextInstance* currentTextInstance = myTextInputs[myCurrentTextInput].myTextInstance;
		oldString = currentTextInstance->GetTextLines()[0];
		currentTextInstance->SetTextLine(0, oldString + L"I");
	}

	myManager.Render();

	if (myCurrentTextInput >= 0 && myBlinkeyBool == true)
	{
		CTextInstance* currentTextInstance = myTextInputs[myCurrentTextInput].myTextInstance;
		currentTextInstance->SetTextLine(0, oldString);
	}

	if (otherOldStringIndex > -1)
	{
		myTextInputs[otherOldStringIndex].myTextInstance->SetTextLine(0, otherOldString);
	}
}

void CMenuState::OnEnter(const bool aLetThroughRender)
{

	myManager.UpdateMousePosition(myManager.GetMopusePosition());
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eCharPressed);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
	myIsInFocus = true;
}

void CMenuState::OnExit(const bool aLetThroughRender)
{
	RENDERER.ClearGui();
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
	myIsInFocus = false;
}

CU::eInputReturn CMenuState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	if (myIsInFocus == false)
	{
		return CU::eInputReturn::ePassOn;
	}
	switch (aInputMessage.myType)
	{
	case CU::eInputType::eMouseMoved:
		myManager.UpdateMousePosition(myManager.GetMopusePosition() + aInputMessage.myMouseDelta);
		break;
	case CU::eInputType::eMousePressed:
		if (aInputMessage.myMouseButton == CU::eMouseButtons::LBUTTON)
		{
			myManager.MousePressed();
		}
		break;
	case CU::eInputType::eMouseReleased:
		if (aInputMessage.myMouseButton == CU::eMouseButtons::LBUTTON)
		{
			myManager.MouseReleased();
		}
		break;
	case CU::eInputType::eScrollWheelChanged: break;
	case CU::eInputType::eKeyboardPressed:
		if (aInputMessage.myKey == CU::eKeys::F7)
		{
			PushTempLobby("");
		}
		if (aInputMessage.myKey == CU::eKeys::BACK && myCurrentTextInput > -1)
		{
			CTextInstance& currentTextInput = *myTextInputs[myCurrentTextInput].myTextInstance;
			currentTextInput.SetTextLine(0, currentTextInput.GetTextLines()[0].substr(0, currentTextInput.GetTextLines()[0].length() - 1));
		}
		break;
	case CU::eInputType::eKeyboardReleased: break;
	default: break;
	}

	return CU::eInputReturn::eKeepSecret;
}

eMessageReturn CMenuState::DoEvent(const KeyCharPressed& aCharPressed)
{
	if (myCurrentTextInput < 0)
	{
		return eMessageReturn::eContinue;
	}

	CTextInstance* textInstance = myTextInputs.At(myCurrentTextInput).myTextInstance;


	if (aCharPressed.GetKey() != 0x08 && aCharPressed.GetKey() != 0x0D)
	{
		std::string blä;
		blä = aCharPressed.GetKey();
		textInstance->SetTextLine(0, textInstance->GetTextLines().At(0) + CU::StringToWString(blä));
	}

	return eMessageReturn::eContinue;
}

eMessageReturn CMenuState::DoEvent(const CConectedMessage& aCharPressed)
{
	return eMessageReturn::eContinue;
}

eMessageReturn CMenuState::DoEvent(const CLoadLevelMessage& aLoadLevelMessage)
{
	myStateStack.PushState(new CLoadState(myStateStack, aLoadLevelMessage.myLevelIndex));
	return eMessageReturn::eContinue;

}

eAlignment CMenuState::LoadAlignment(const CU::CJsonValue& aJsonValue)
{
	if (aJsonValue.GetString() == "center")
	{
		return eAlignment::eCenter;
	}
	if (aJsonValue.GetString() == "right")
	{
		return eAlignment::eRight;
	}
	return eAlignment::eLeft;
}

void CMenuState::LoadElement(const CU::CJsonValue& aJsonValue, const std::string &aFolderpath)
{
	const std::string &name = aJsonValue.at("name").GetString();
	const CU::Vector2f position = aJsonValue.at("position").GetVector2f("xy");
	const CU::Vector2f origin = aJsonValue.at("origin").GetVector2f("xy");

	const int spriteID = myManager.CreateSprite(aFolderpath + "/" + name, position, origin, 1);

	if (aJsonValue.at("isButton").GetBool())
	{
		const CU::CJsonValue &buttonValue = aJsonValue.at("button");

		CU::CJsonValue actionArray = buttonValue.at("actions");

		CU::GrowingArray<std::string> actions(MAX(actionArray.Size(), 1));
		CU::GrowingArray<std::string> arguments(MAX(actionArray.Size(), 1));

		for (int i = 0; i < actionArray.Size(); ++i)
		{
			actions.Add(actionArray.at(i).at("type").GetString());
			arguments.Add(actionArray.at(i).at("argument").GetString());
		}

		myManager.CreateClickArea(actions, arguments, spriteID, buttonValue.at("rect").GetVector4f("xyzw"), 1);
	}

	if (aJsonValue.at("hasText").GetBool())
	{
		const CU::CJsonValue &textValue = aJsonValue.at("text");
		std::string fontName = textValue.at("font").GetString();
		std::wstring text = CU::StringToWString(textValue.at("text").GetString());

		eAlignment alignment = LoadAlignment(textValue.at("alignment"));

		CU::Vector2f textPosition;
		if (spriteID < 0)
		{
			textPosition = position + textValue.at("offset").GetVector2f();
		}
		else
		{
			const SMenuSprite& currentSprite = myManager.GetSprite(spriteID);
			textPosition = position + (textValue.at("offset").GetVector2f() - currentSprite.myDafaultSprite->GetPivot()) * currentSprite.myDafaultSprite->GetSize();
		}

		if (text.size() > 0 && text.at(0) == L'#')
		{
			std::wstring::size_type underscore = text.find(L"_");
			if (underscore == std::wstring::npos)
			{
				underscore = text.length() + 1;
			}

			std::wstring subString = text.substr(1, underscore - 1);

			if (subString == L"textInput")
			{
				const std::wstring numberString = text.substr(underscore + 1, text.size() - underscore + 1);
				const int currentTextInput = std::stoi(numberString);

				while (myTextInputs.Size() < currentTextInput + 1)
				{
					myTextInputs.Add(STextInput());
				}

				const int textInputTextInstanceIndex = myManager.CreateText(fontName, textPosition, L"", 2, alignment);
				myTextInputs[currentTextInput].myTextInstance = myManager.GetTextInstance(textInputTextInstanceIndex);
				myTextInputs[currentTextInput].myTextInstance->SetColor({ 0.f,0.f,0.f,1.f });
			}
			else if (subString == L"IP")
			{
				GetIPAddress();
				myManager.GetTextInstance(myManager.CreateText(fontName, textPosition, myThisComputersIP, 2, alignment))->SetColor({0.f, 0.f, 0.f , 1.f});
			}
		}
		else
		{
			myManager.CreateText(fontName, textPosition, text, 2, alignment);
		}
	}
}

void CMenuState::MenuLoad(const std::string& aFile)
{
	CU::CJsonValue root;
	root.Parse(aFile);

	myManager.SetMousePointer(new CSpriteInstance(root.at("cursor").GetString().c_str()));
	const std::string &folderPath = root.at("folder").GetString();
	myShowStateBelow = root.at("letThroughRender").GetBool();

	const CU::CJsonValue elementArray = root.at("elements");
	for (unsigned i = 0; i < elementArray.Size(); ++i)
	{
		LoadElement(elementArray.at(i), folderPath);
	}
}

bool CMenuState::PushMenu(std::string aMenu) 
{
	myStateStack.PushState(new CMenuState(myStateStack, aMenu));
	return true;
}

bool CMenuState::ExitGame(std::string /* not used*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CQuitGame);
	return true;
}

bool CMenuState::PushTempLobby(std::string /*notUsed*/) 
{
	myStateStack.PushState(new CTempLobbyState(myStateStack));
	return true;
}

bool CMenuState::PopMenues(std::string aNumberOfMenues)
{
	ourMenuesToPop = stoi(aNumberOfMenues);
	return true;
}

bool CMenuState::PushLevel(std::string aLevelIndexString)
{
	myStateStack.PushState(new CLoadState(myStateStack, std::stoi(aLevelIndexString)));
	CNetworkMessage_LoadLevel* netowrkMessageMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_LoadLevel>("__All_But_Me");
	netowrkMessageMessage->myLevelIndex = std::stoi(aLevelIndexString);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(netowrkMessageMessage));
	RENDERER.ClearGui();
	return true;
}

bool CMenuState::StartServer(std::string /*notUsed*/)
{
	std::string processName = "TServer_Applictaion_x64_";

#ifdef _DEBUG
	processName += "Debug";
#elif defined(RETAIL)
	processName += "Retail";
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
	return true;
}

bool CMenuState::ConnectLocal(std::string anIp)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CConectMessage(myName, "127.0.0.1"));
	return true;
}

bool CMenuState::SetCurrentTextInput(std::string aTexINputIndex)
{
	myCurrentTextInput = stoi(aTexINputIndex);
	myTextInputs[myCurrentTextInput].myInputIsValid = true;
	return true;
}

bool CMenuState::CheckIp(std::string aTextInput)
{
	const std::wstring ipCheck = L"1234567890.";

	const int index = stoi(aTextInput);

	CTextInstance& textInstance = *myTextInputs[index].myTextInstance;

	for (int i = 0; i < textInstance.GetTextLines()[0].length(); ++i)
	{
		if (ipCheck.find(textInstance.GetTextLines()[0].at(i)) == std::wstring::npos)
		{
			myTextInputs[index].myInputIsValid = false;
			myCurrentTextInput = -1;
			return false;
		}
	}

	return true;
}

bool CMenuState::SetName(std::string aTextInput)
{
	const int index = stoi(aTextInput);
	myName = CU::StringHelper::WStringToString(myTextInputs[index].myTextInstance->GetTextLines()[0]);
	return true;
}

bool CMenuState::SetIp(std::string aTextInput)
{
	const int index = stoi(aTextInput);
	myIp = CU::StringHelper::WStringToString(myTextInputs[index].myTextInstance->GetTextLines()[0]);
	return true;
}

bool CMenuState::Conect(std::string aTextInput)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CConectMessage(myName, myIp));
	return true;
}


#include <Iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

void CMenuState::GetIPAddress()
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
	} while (result == ERROR_BUFFER_OVERFLOW);
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
		myThisComputersIP = L"error";
	}
}