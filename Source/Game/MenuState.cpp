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

CMenuState::CMenuState(StateStack& aStateStack, std::string aFile): State(aStateStack, eInputMessengerType::eMainMenu), myShowStateBelow(false), myPointerSprite(nullptr)
{
	myManager.AddAction("ExitGame", bind(&CMenuState::ExitGame, std::placeholders::_1));
	myManager.AddAction("PushTempLobby", bind(&CMenuState::PushTempLobby, this, std::placeholders::_1));

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
	return eStateStatus::eKeep;
}

void CMenuState::Render()
{
	myManager.Render();
}

void CMenuState::OnEnter(const bool aLetThroughRender)
{
}

void CMenuState::OnExit(const bool aLetThroughRender)
{
	RENDERER.ClearGui();
}

CU::eInputReturn CMenuState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
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

	return CU::eInputReturn::ePassOn;
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

void CMenuState::ExitGame(std::string /* not used*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CQuitGame);
}

void CMenuState::PushTempLobby(std::string /*notUsed*/) const
{

	myStateStack.PushState(new CTempLobbyState(myStateStack)); 
}
