#include "stdafx.h"
#include "MenuState.h"
#include "InputMessage.h"
#include "EInputReturn.h"
#include "JsonValue.h"
#include "EMouseButtons.h"
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

char CMenuState::ourMenuesToPop = 0;

CMenuState::CMenuState(StateStack& aStateStack, std::string aFile): State(aStateStack, eInputMessengerType::eMainMenu), myShowStateBelow(false), myPointerSprite(nullptr),myIsInFocus(false)
{
	myManager.AddAction("ExitGame", bind(&CMenuState::ExitGame, std::placeholders::_1));
	myManager.AddAction("PushTempLobby", bind(&CMenuState::PushTempLobby, this, std::placeholders::_1));
	myManager.AddAction("PushMenu", bind(&CMenuState::PushMenu, this, std::placeholders::_1));
	myManager.AddAction("PopMenues", bind(&CMenuState::PopMenues, this, std::placeholders::_1));
	myManager.AddAction("PushLevel", bind(&CMenuState::PushLevel, this, std::placeholders::_1));
	myManager.AddAction("StartServer", bind(&CMenuState::StartServer, std::placeholders::_1));
	myManager.AddAction("ConectLocal", bind(&CMenuState::ConnectLocal, std::placeholders::_1));

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
	myManager.Render();
}

void CMenuState::OnEnter(const bool aLetThroughRender)
{
	myIsInFocus = true;
}

void CMenuState::OnExit(const bool aLetThroughRender)
{
	RENDERER.ClearGui();
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
	case CU::eInputType::eKeyboardPressed: break;
	case CU::eInputType::eKeyboardReleased: break;
	default: break;
	}

	return CU::eInputReturn::eKeepSecret;
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

void CMenuState::LoadElement(const CU::CJsonValue& aJsonValue,const std::string &aFolderpath)
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
		if (spriteID <0)
		{
			textPosition = position + textValue.at("offset").GetVector2f();
		}
		else
		{
			const SMenuSprite& currentSprite = myManager.GetSprite(spriteID);
			textPosition = position + (textValue.at("offset").GetVector2f()  - currentSprite.myDafaultSprite->GetPivot()) * currentSprite.myDafaultSprite->GetSize();
		}

		myManager.CreateText(fontName, textPosition, text, 2, alignment);
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

void CMenuState::PushMenu(std::string aMenu) const
{
	myStateStack.PushState(new CMenuState(myStateStack, aMenu));
}

void CMenuState::ExitGame(std::string /* not used*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CQuitGame);
}

void CMenuState::PushTempLobby(std::string /*notUsed*/) const
{
	myStateStack.PushState(new CTempLobbyState(myStateStack)); 
}

void CMenuState::PopMenues(std::string aNumberOfMenues) const
{
	ourMenuesToPop = stoi(aNumberOfMenues);
}

void CMenuState::PushLevel(std::string aLevelIndexString) const
{
	myStateStack.PushState(new CLoadState(myStateStack, std::stoi(aLevelIndexString)));
	CNetworkMessage_LoadLevel* netowrkMessageMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_LoadLevel>("__All_But_Me");
	netowrkMessageMessage->myLevelIndex = std::stoi(aLevelIndexString);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(netowrkMessageMessage));
}

void CMenuState::StartServer(std::string /*notUsed*/)
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
}

void CMenuState::ConnectLocal(std::string anIp)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CConectMessage("YouAreAlone", "127.0.0.1"));
}
