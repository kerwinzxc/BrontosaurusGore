#include "stdafx.h"
#include "MenuState.h"
#include "InputMessage.h"
#include "EInputReturn.h"
#include "JsonValue.h"

CMenuState::CMenuState(StateStack& aStateStack, std::string aFile): State(aStateStack, eInputMessengerType::eMainMenu), myShowStateBelow(false), myPointerSprite(nullptr)
{
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
}

CU::eInputReturn CMenuState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	switch (aInputMessage.myType)
	{
	case CU::eInputType::eMouseMoved: 
		myManager.UpdateMousePosition(myManager.GetMopusePosition() + aInputMessage.myMouseDelta);

		break;
	case CU::eInputType::eMousePressed: break;
	case CU::eInputType::eMouseReleased: break;
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
		myManager.CreateClickArea(0, spriteID, buttonValue.at("rect").GetVector4f("xyzw"), 1);
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
