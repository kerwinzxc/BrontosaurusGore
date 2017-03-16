#include "stdafx.h"
#include "HUD.h"
#include "JsonValue.h"


CHUD::CHUD(): myHealthAndArmorSprite(nullptr), myHealthBar(nullptr), myArmourBar(nullptr), myWeaponSprite(nullptr), myCrosshairSprite(nullptr)
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
}
