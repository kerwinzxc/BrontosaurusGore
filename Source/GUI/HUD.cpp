#include "stdafx.h"
#include "HUD.h"
#include "JsonValue.h"
#include "BrontosaurusEngine/SpriteInstance.h"
#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/Renderer.h"
#include "CommonUtilities.h"
#include "BarInstance.h"
#include <algorithm>


CHUD::CHUD() : myHealthAndArmorSprite(nullptr), myHealthBar(nullptr), myArmourBar(nullptr), myHealthAndArmourHasChanged(true), myWeaponSprite(nullptr), myCurrentWeapon(0), myTransitionLength(0), myWeaponHUDHasChanged(true), myCrosshairSprite(nullptr), testValue(0.f)
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
	LoadWeaponHud(jsonDocument.at("weapon"));
}

void CHUD::LoadArmourAndHealth(const CU::CJsonValue& aJsonValue)
{
	const std::string backgroundPath = aJsonValue.at("background").GetString();
	myHealthAndArmorSprite = new CSpriteInstance(backgroundPath.c_str(), CU::Vector2f(1.f, 1.f));

	myHealthAndArmourElement = LoadElement(aJsonValue);

	LoadText(aJsonValue.at("armourNumber"), myArmourNumber);
	LoadText(aJsonValue.at("healthNumber"), myHealthNumber);

	myArmourNumber.SetTextLine(0, L"0");
	myHealthNumber.SetTextLine(0, L"1");

	myArmourBar = LoadBar(aJsonValue.at("armorBar"));
	myHealthBar = LoadBar(aJsonValue.at("healthBar"));
}

void CHUD::LoadWeaponHud(const CU::CJsonValue& aJsonValue)
{
	myWeaponElement = LoadElement(aJsonValue);

	const CU::CJsonValue & spretsheeValue = aJsonValue.at("spritesheet");

	const std::string spritePath = spretsheeValue.at("file").GetString();

	myWeaponSprite = new CSpriteInstance(spritePath.c_str());

	const CU::CJsonValue & frameSizeValue = spretsheeValue.at("frameSize");
	const float frameWidth = frameSizeValue.at("width").GetFloat();
	const float frameHeight = frameSizeValue.at("height").GetFloat();

	const CU::Vector2f mySpriteSize = myWeaponSprite->GetTextureSize();

	myWeaponSprite->SetRect({ 0,0, frameWidth / mySpriteSize.x, frameHeight / mySpriteSize.y });
	myWeaponSprite->SetSize({ 1.f,1.f });

	const CU::CJsonValue & weaponIndexValue = spretsheeValue.at("weaponTypes");

	for (int i = 0; i < weaponIndexValue.Size(); ++i)
	{
		myWeaponIndexes[weaponIndexValue.at(i).GetString()] = i;
	}

	myCurrentWeapon = 0;
	myTransitionLength = spretsheeValue.at("transitionLength").GetInt();
	myPickedUpWeapons = 0;

	LoadText(aJsonValue.at("ammoText"), myAmmoNumber);
	myAmmoNumber.SetText(L"\u221E");
}

void CHUD::Update(CU::Time aDeltaTime)
{
	myHealthBar->SetLevel(MAX(myHealthBar->GetLevel() - 0.01, 0));
}

void CHUD::SetAmmoHudRect()
{
	unsigned offset = 0;

	switch (myPickedUpWeapons.GetBits())
	{
	case 1 << 1:
		offset = 1;
		break;
	case 1 << 2:
		offset = 3;
		break;
	case 1 << 3:
		offset = 5;
		break;
	case 1 << 1 | 1 << 2:
		offset = 7;
		break;
	case 1 << 1 | 1 << 3:
		offset = 10;
		break;
	case 1 << 2 | 1 << 3:
		offset = 13;
		break;
	case 1 << 1 | 1 << 2 | 1 << 3:
		offset = 16;
		break;
	default: break;
	}

	const unsigned immageIndex = myCurrentWeapon + offset;

	const CU::Vector4f &currentRect = myWeaponSprite->GetRect();
	const CU::Vector2f frameSize = CU::Vector2f(currentRect.z - currentRect.x, currentRect.w - currentRect.y);

	const float newX = frameSize.x * immageIndex;
	const int newRow = newX;

	const int largeX = newX * 1000;
	const float finalX = static_cast<float>(largeX % 1000) / 1000;
	const float finalY = static_cast<float>(newRow) * frameSize.y;

	CU::Vector4f rect(CU::Vector4f(MIN(finalX, 1), MIN(1 - (finalY + frameSize.y), 1), MIN(finalX + frameSize.x, 1), MIN(1 - finalY, 1)));
	myWeaponSprite->SetRect(rect);
}

void CHUD::Render()
{
	if (myHealthAndArmourHasChanged == true)
	{
		SCreateOrClearGuiElement* createOrClearGui = new SCreateOrClearGuiElement(L"healthAndArmour", myHealthAndArmourElement.myGuiElement, myHealthAndArmourElement.myPixelSize);

		RENDERER.AddRenderMessage(createOrClearGui);

		myHealthAndArmorSprite->RenderToGUI(L"healthAndArmour");
		myHealthBar->RenderToGUI(L"healthAndArmour");
		myArmourBar->RenderToGUI(L"healthAndArmour");
		myHealthNumber.RenderToGUI(L"healthAndArmour");
		myArmourNumber.RenderToGUI(L"healthAndArmour");
		myHealthAndArmourHasChanged = false;
	}

	if (myWeaponHUDHasChanged == true)
	{
		SetAmmoHudRect();

		SCreateOrClearGuiElement* createOrClearGui = new SCreateOrClearGuiElement(L"weapon", myWeaponElement.myGuiElement, myWeaponElement.myPixelSize);

		RENDERER.AddRenderMessage(createOrClearGui);

		myWeaponSprite->RenderToGUI(L"weapon");
		myAmmoNumber.RenderToGUI(L"weapon");
		//myWeaponHUDHasChanged = false;
	}
}

SHUDElement CHUD::LoadElement(const CU::CJsonValue& aJsonValue) const
{
	SHUDElement hudElement;

	hudElement.myGuiElement.myOrigin = aJsonValue.at("origin").GetVector2f();

	hudElement.myGuiElement.myAnchor = 0;

	const CU::CJsonValue anchorArray = aJsonValue.at("anchor");
	for (int i = 0; i < anchorArray.Size(); ++i)
	{
		std::string anchorValue = anchorArray.at(i).GetString();
		std::transform(anchorValue.begin(), anchorValue.end(), anchorValue.begin(), ::tolower);
		if (anchorValue == "top")
		{
			hudElement.myGuiElement.myAnchor[static_cast<int>(eAnchors::eTop)] = true;
		}
		else if (anchorValue == "bottom")
		{
			hudElement.myGuiElement.myAnchor[static_cast<int>(eAnchors::eBottom)] = true;
		}
		else if (anchorValue == "left")
		{
			hudElement.myGuiElement.myAnchor[static_cast<int>(eAnchors::eLeft)] = true;
		}
		else if (anchorValue == "right")
		{
			hudElement.myGuiElement.myAnchor[static_cast<int>(eAnchors::eRight)] = true;
		}
	}

	hudElement.myGuiElement.myScreenRect = CU::Vector4f(aJsonValue.at("position").GetVector2f());

	const CU::CJsonValue sizeObject = aJsonValue.at("Size");

	const unsigned pixelWidth = sizeObject.at("pixelWidth").GetUInt();
	const unsigned pixelHeight = sizeObject.at("pixelHeight").GetUInt();
	hudElement.myPixelSize = CU::Vector2ui(pixelWidth, pixelHeight);

	hudElement.myGuiElement.myScreenRect.z = sizeObject.at("screenSpaceWidth").GetFloat() + hudElement.myGuiElement.myScreenRect.x;
	hudElement.myGuiElement.myScreenRect.w = sizeObject.at("screenSpaceHeight").GetFloat() + hudElement.myGuiElement.myScreenRect.y;

	return hudElement;
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
