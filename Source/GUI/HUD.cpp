#include "stdafx.h"
#include "HUD.h"
#include "JsonValue.h"
#include "BrontosaurusEngine/SpriteInstance.h"
#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/Renderer.h"
#include "CommonUtilities.h"
#include "BarInstance.h"
#include <algorithm>
#include "Game/PollingStation.h"
#include "Components/ComponentAnswer.h"
#include "Components/GameObject.h"
#include "Components/Weapon.h"
#include "Components/WeaponData.h"


CHUD::CHUD() : myHealthAndArmorSprite(nullptr),
myCurrentHealth(0),
myCurrentMaxHealth(0),
myCurrentArmour(0),
myCurrentMaxArmour(0),
myHealthBar(nullptr),
myArmourBar(nullptr),
myHealthAndArmourHasChanged(true),
myWeaponSprite(nullptr),
myCurrentWeapon(0),
myTransitionLength(0),
myWeaponHUDHasChanged(true),
myActiveCrosshairSprite(nullptr),
myCrosshairHasUpdated(true)
{
	myCrosshairSprites.Init(4);
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
	LoadCrosshair(jsonDocument.at("crosshairs"));
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

	const CU::CJsonValue& spretsheeValue = aJsonValue.at("spritesheet");

	const std::string spritePath = spretsheeValue.at("file").GetString();

	myWeaponSprite = new CSpriteInstance(spritePath.c_str());

	const CU::CJsonValue& frameSizeValue = spretsheeValue.at("frameSize");

	const CU::Vector2f mySpriteSize = myWeaponSprite->GetTextureSize();

	myFrameSize.x = frameSizeValue.at("width").GetFloat() / mySpriteSize.x;
	myFrameSize.y = frameSizeValue.at("height").GetFloat() / mySpriteSize.y;

	myWeaponSprite->SetRect({ 0,0, myFrameSize.x, myFrameSize.y});
	myWeaponSprite->SetSize({ 1.f,1.f });

	const CU::CJsonValue& weaponIndexValue = spretsheeValue.at("weaponTypes");

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

void CHUD::LoadCrosshair(const CU::CJsonValue& aJsonValue)
{
	// For now I'm just using the same element for all sprites; change if nessesary.

	//Kyle
	CU::CJsonValue jsonDocument;
	jsonDocument.Parse("Json/HUD/HUD.json");
	const CU::CJsonValue chrosshairArray = jsonDocument.at("crosshairs");
	myCrosshairElement = LoadElement(chrosshairArray.at(0));

	for (int i = 0; i < chrosshairArray.Size(); ++i)
	{
		CSpriteInstance* sprite = new CSpriteInstance(chrosshairArray.at(i).at("sprite").GetString().c_str());
		myCrosshairSprites.Add(sprite);
		myCrosshairSprites[i]->SetSize({ 1.f,1.f });
	}
	myActiveCrosshairSprite = myCrosshairSprites[0];
}

void CHUD::UpdateHealthAndArmour()
{
	SComponentQuestionData healthData;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetHealth, healthData);
	SComponentQuestionData maxHealthData;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetMaxHealth, maxHealthData);

	if (healthData.myInt != myCurrentHealth | maxHealthData.myInt != myCurrentMaxHealth)
	{
		myCurrentHealth = healthData.myInt;
		myCurrentMaxHealth = maxHealthData.myInt;

		std::wstring numberString = std::to_wstring(myCurrentHealth);
		numberString += L"/";
		numberString += std::to_wstring(myCurrentMaxHealth);

		myHealthNumber.SetText(numberString);
		myHealthBar->SetLevel(myCurrentHealth / myCurrentMaxHealth);

		myHealthAndArmourHasChanged = true;
	}

	SComponentQuestionData armourData;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetArmor, armourData);
	SComponentQuestionData maxArmourData;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetMaxArmor, maxArmourData);

	if (armourData.myInt != myCurrentArmour | maxArmourData.myInt != myCurrentMaxArmour)
	{
		myCurrentArmour = armourData.myInt;
		myCurrentMaxArmour = maxArmourData.myInt;

		std::wstring armourString = std::to_wstring(myCurrentArmour);
		armourString += L"/";
		armourString += std::to_wstring(myCurrentMaxArmour);

		myArmourNumber.SetText(armourString);
		myArmourBar->SetLevel(myCurrentArmour / myCurrentMaxArmour);

		myHealthAndArmourHasChanged = true;
	}
}

void CHUD::UpdateWeapon()
{
	SComponentQuestionData ammoQuestion;
	ammoQuestion.myAmmoLeftData = new SAmmoLeftData;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetCurrentAmmoData, ammoQuestion);

	if (ammoQuestion.myAmmoLeftData->ammoLeft != myCurrentAmmo || ammoQuestion.myAmmoLeftData->maxAmmo != myCurrentMaxAmmo)
	{
		myCurrentAmmo = ammoQuestion.myAmmoLeftData->ammoLeft;
		myCurrentMaxAmmo = ammoQuestion.myAmmoLeftData->maxAmmo;

		std::wstring ammoString = std::to_wstring(myCurrentAmmo);
		ammoString += L"/";
		ammoString += std::to_wstring(myCurrentMaxAmmo);
		myAmmoNumber.SetText(ammoString);
		myWeaponHUDHasChanged = true;
	}

	if (myWeaponIndexes.at(ammoQuestion.myAmmoLeftData->weaponName) != myCurrentWeapon)
	{
		myCurrentWeapon = myWeaponIndexes.at(ammoQuestion.myAmmoLeftData->weaponName);
		myActiveCrosshairSprite = myCrosshairSprites[myCurrentWeapon];
	}
	delete ammoQuestion.myAmmoLeftData;

	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetWeapons, ammoQuestion);

	if (myAmmountOfWeapons != ammoQuestion.myWeapons->Size())
	{
		for (int i = 0; i < ammoQuestion.myWeapons->Size(); ++i)
		{
			const std::string weaponName = ammoQuestion.myWeapons->At(i)->GetData()->name;
			const int weaponIndex = myWeaponIndexes.at(weaponName);

			myAmmountOfWeapons = ammoQuestion.myWeapons->Size();

			if (weaponIndex > 0)
			{
				myPickedUpWeapons[weaponIndex - 1] = true;
			}
		}
	}
}

void CHUD::Update(CU::Time aDeltaTime)
{
	UpdateHealthAndArmour();
	UpdateWeapon();
}

void CHUD::SetAmmoHudRect()
{
	unsigned imageIndex = 0;

	/*switch (myPickedUpWeapons.GetBits())
	{
	case 1 << 1:
		imageIndex = 1 + myCurrentWeapon;
		break;
	case 1 << 2:
		imageIndex = 3;

		if (myCurrentWeapon > 0)
		{
			imageIndex += 1;
		}
		break;
	case 1 << 3:
		imageIndex = 5;

		if (myCurrentWeapon > 0)
		{
			imageIndex += 1;
		}
		break;
	case 1 << 1 | 1 << 2:
		imageIndex = 7;
		break;
	case 1 << 1 | 1 << 3:
		imageIndex = 10;

		if (myCurrentWeapon > 1)
		{
			imageIndex += 2;
		}
		else
		{
			imageIndex += myCurrentWeapon;
		}
		break;
	case 1 << 2 | 1 << 3:
		imageIndex = 13;

		if (myCurrentWeapon > 0)
		{
			imageIndex += myCurrentWeapon - 1;
		}
		break;
	case 1 << 1 | 1 << 2 | 1 << 3:
		imageIndex = 16 + myCurrentWeapon;
		break;
	default: break;
	}*/
	//TODO: create a simple way to use a bitset in a switch :smile:

	if (myPickedUpWeapons.All())
	{
		imageIndex = 16 + myCurrentWeapon;
	}
	else if (myPickedUpWeapons[1] && myPickedUpWeapons[2])
	{
		imageIndex = 13;

		if (myCurrentWeapon > 0)
		{
			imageIndex += myCurrentWeapon - 1;
		}
	}
	else if (myPickedUpWeapons[0] && myPickedUpWeapons[2])
	{
		imageIndex = 10;

		if (myCurrentWeapon > 1)
		{
			imageIndex += 2;
		}
		else
		{
			imageIndex += myCurrentWeapon;
		}
	}
	else if (myPickedUpWeapons[0] && myPickedUpWeapons[1])
	{
		imageIndex = 7;
	}
	else if (myPickedUpWeapons[0])
	{
		imageIndex = 1 + myCurrentWeapon;
	}
	else if (myPickedUpWeapons[1])
	{
		imageIndex = 3;

		if (myCurrentWeapon > 0)
		{
			imageIndex += 1;
		}
	}
	else if (myPickedUpWeapons[2])
	{
		imageIndex = 5;

		if (myCurrentWeapon > 0)
		{
			imageIndex += 1;
		}
	}
	
	//const unsigned immageIndex = myCurrentWeapon + offset;

	const CU::Vector2f &frameSize = myFrameSize;

	const float newX = frameSize.x * imageIndex;
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

		SChangeStatesMessage* changeStatesMessage = new SChangeStatesMessage();
		changeStatesMessage->myBlendState = eBlendState::eEmilBlend;
		changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
		changeStatesMessage->mySamplerState = eSamplerState::eClamp;

		SRenderToGUI* guiChangeState = new SRenderToGUI(L"healthAndArmour", changeStatesMessage);
		RENDERER.AddRenderMessage(guiChangeState);

		myHealthAndArmorSprite->RenderToGUI(L"healthAndArmour");

		 changeStatesMessage = new SChangeStatesMessage();
		changeStatesMessage->myBlendState = eBlendState::eEndBlend;
		changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
		changeStatesMessage->mySamplerState = eSamplerState::eClamp;

		guiChangeState = new SRenderToGUI(L"healthAndArmour", changeStatesMessage);
		RENDERER.AddRenderMessage(guiChangeState);

		myHealthBar->RenderToGUI(L"healthAndArmour");
		myArmourBar->RenderToGUI(L"healthAndArmour");
		myHealthNumber.RenderToGUI(L"healthAndArmour");
		myArmourNumber.RenderToGUI(L"healthAndArmour");
		//myHealthAndArmourHasChanged = false;
	}

	if (myWeaponHUDHasChanged == true)
	{
		SetAmmoHudRect();

		SCreateOrClearGuiElement* createOrClearGui = new SCreateOrClearGuiElement(L"weapon", myWeaponElement.myGuiElement, myWeaponElement.myPixelSize);

		RENDERER.AddRenderMessage(createOrClearGui);

		SChangeStatesMessage* changeStatesMessage = new SChangeStatesMessage();
		changeStatesMessage->myBlendState = eBlendState::eEmilBlend;
		changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
		changeStatesMessage->mySamplerState = eSamplerState::eClamp;

		SRenderToGUI* guiChangeState = new SRenderToGUI(L"weapon", changeStatesMessage);
		RENDERER.AddRenderMessage(guiChangeState);

		myWeaponSprite->RenderToGUI(L"weapon");

		changeStatesMessage = new SChangeStatesMessage();
		changeStatesMessage->myBlendState = eBlendState::eEndBlend;
		changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
		changeStatesMessage->mySamplerState = eSamplerState::eClamp;

		guiChangeState = new SRenderToGUI(L"healthAndArmour", changeStatesMessage);
		RENDERER.AddRenderMessage(guiChangeState);

		myAmmoNumber.RenderToGUI(L"weapon");
		//myWeaponHUDHasChanged = false;
	}

	if (myCrosshairHasUpdated == true)
	{
		SCreateOrClearGuiElement* createOrClearGui = new SCreateOrClearGuiElement(L"crosshairs", myCrosshairElement.myGuiElement, myCrosshairElement.myPixelSize);

		RENDERER.AddRenderMessage(createOrClearGui);

		SChangeStatesMessage* const changeStatesMessage = new SChangeStatesMessage();
		changeStatesMessage->myBlendState = eBlendState::eAddBlend;
		changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
		changeStatesMessage->mySamplerState = eSamplerState::eClamp;

		SRenderToGUI*const guiChangeState = new SRenderToGUI(L"crosshairs", changeStatesMessage);
		RENDERER.AddRenderMessage(guiChangeState);

		//myCrosshairSprite->RenderToGUI(L"crosshairs");
		myActiveCrosshairSprite->RenderToGUI(L"crosshairs");
		//myCrosshairHasUpdated = false;
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
	const CU::Vector4f flipedRect(rect.x, 1 - rect.w, rect.z, 1 - rect.y);

	return new CBarInstance(backgroundColour, fullColour, emptyColour, flipedRect);
}
