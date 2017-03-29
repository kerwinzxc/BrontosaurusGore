#include "stdafx.h"
#include "HUD.h"
#include "JsonValue.h"
#include "BrontosaurusEngine/SpriteInstance.h"
#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/Renderer.h"
#include "CommonUtilities.h"
#include "BarInstance.h"
#include <algorithm>


CHUD::CHUD() : myHealthAndArmorSprite(nullptr), myHealthBar(nullptr), myArmourBar(nullptr), myHealthAndArmourHasChanged(true), myWeaponSprite(nullptr), myCrosshairSprite(nullptr), testValue(0.f)
{
}


CHUD::~CHUD()
{
}

void CHUD::LoadHUD()
{
	CU::CJsonValue jsonDocument;

	jsonDocument.Parse("Json/HUD/HUD.json");

	LoadArmourAndHealth(jsonDocument.at("healthAndArmor"));
}

void CHUD::LoadArmourAndHealth(const CU::CJsonValue& aJsonValue)
{
	const std::string backgroundPath = aJsonValue.at("background").GetString();
	myHealthAndArmorSprite = new CSpriteInstance(backgroundPath.c_str(), CU::Vector2f(1.f, 1.f));

	myHealthAndArmourElement.myGuiElement.myOrigin = aJsonValue.at("origin").GetVector2f();

	myHealthAndArmourElement.myGuiElement.myAnchor = 0;

	const CU::CJsonValue anchorArray = aJsonValue.at("anchor");
	for (int i = 0; i < anchorArray.Size(); ++i)
	{
		std::string anchorValue = anchorArray.at(i).GetString();
		std::transform(anchorValue.begin(), anchorValue.end(), anchorValue.begin(), ::tolower);
		if (anchorValue == "top")
		{
			myHealthAndArmourElement.myGuiElement.myAnchor[static_cast<int>(eAnchors::eTop)] = true;
		}
		else if (anchorValue == "bottom")
		{
			myHealthAndArmourElement.myGuiElement.myAnchor[static_cast<int>(eAnchors::eBottom)] = true;
		}
		else if (anchorValue == "left")
		{
			myHealthAndArmourElement.myGuiElement.myAnchor[static_cast<int>(eAnchors::eLeft)] = true;
		}
		else if (anchorValue == "right")
		{
			myHealthAndArmourElement.myGuiElement.myAnchor[static_cast<int>(eAnchors::eRight)] = true;
		}
	}

	myHealthAndArmourElement.myGuiElement.myScreenRect = CU::Vector4f(aJsonValue.at("position").GetVector2f());

	const CU::CJsonValue sizeObject = aJsonValue.at("Size");

	const unsigned pixelWidth = sizeObject.at("pixelWidth").GetUInt();
	const unsigned pixelHeight = sizeObject.at("pixelHeight").GetUInt();
	myHealthAndArmourElement.myPixelSize = CU::Vector2ui(pixelWidth, pixelHeight);

	myHealthAndArmourElement.myGuiElement.myScreenRect.z = sizeObject.at("screenSpaceWidth").GetFloat() + myHealthAndArmourElement.myGuiElement.myScreenRect.x;
	myHealthAndArmourElement.myGuiElement.myScreenRect.w = sizeObject.at("screenSpaceHeight").GetFloat() + myHealthAndArmourElement.myGuiElement.myScreenRect.y;

	LoadText(aJsonValue.at("armourNumber"), myArmourNumber);
	LoadText(aJsonValue.at("healthNumber"), myHealthNumber);

	myAmmoNumber.SetTextLine(0, L"0");
	myHealthNumber.SetTextLine(0, L"0");

	myArmourBar = LoadBar(aJsonValue.at("armorBar"));
	myHealthBar = LoadBar(aJsonValue.at("healthBar"));
}

void CHUD::Update(CU::Time aDeltaTime)
{
	myHealthBar->SetLevel(MAX(myHealthBar->GetLevel() - 0.01, 0));
}

void CHUD::Render()
{
	if (myHealthAndArmourHasChanged == true)
	{
		SCreateOrClearGuiElement* createOrClearGui =new SCreateOrClearGuiElement(L"healthAndArmour", myHealthAndArmourElement.myGuiElement, myHealthAndArmourElement.myPixelSize);

		RENDERER.AddRenderMessage(createOrClearGui);

		myHealthAndArmorSprite->RenderToGUI(L"healthAndArmour");
		myHealthBar->RenderToGUI(L"healthAndArmour");
		myArmourBar->RenderToGUI(L"healthAndArmour");
		myHealthNumber.RenderToGUI(L"healthAndArmour");
		myArmourNumber.RenderToGUI(L"healthAndArmour");
		myHealthAndArmourHasChanged = false;
	}
}

void CHUD::LoadText(const CU::CJsonValue& aJsonValue, CTextInstance& aTextInstance) const
{

	aTextInstance.Init(aJsonValue.at("font").GetString());

	const std::string alignemnt = aJsonValue.at("alignment").GetString();
	if (alignemnt == "Right" || alignemnt == "right")
	{
		aTextInstance.SetAlignment(eAlignment::eRight);
	}
	else if (alignemnt == "Center" || alignemnt == "center")
	{
		aTextInstance.SetAlignment(eAlignment::eCenter);
	}
	else
	{
		aTextInstance.SetAlignment(eAlignment::eLeft);
	}

	aTextInstance.SetColor(aJsonValue.at("colour").GetVector4f("rgba"));
	aTextInstance.SetPosition(aJsonValue.at("position").GetVector2f());
}

CBarInstance* CHUD::LoadBar(const CU::CJsonValue& aJsonValue)
{
	const CU::Colour backgroundColour = aJsonValue.at("backgroundColour").GetVector4f("rgba");
	const CU::Colour fullColour = aJsonValue.at("fullColour").GetVector4f("rgba");
	const CU::Colour emptyColour = aJsonValue.at("emptyColour").GetVector4f("rgba");

	const CU::CJsonValue rectValue = aJsonValue.at("rect");
	const CU::Vector4f rect(rectValue.at("topLeft").GetVector2f(), rectValue.at("bottomRight").GetVector2f());

	return new CBarInstance(backgroundColour, fullColour, emptyColour, rect);
}
