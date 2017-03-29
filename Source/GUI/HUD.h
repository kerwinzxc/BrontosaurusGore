#pragma once
#include "../BrontosaurusEngine/TextInstance.h"
#include "../CommonUtilities/JsonValue.h"
#include "GUIElement.h"

class CSpriteInstance;
class CBarInstance;

struct SHUDElement
{
	SGUIElement myGuiElement;
	CU::Vector2ui myPixelSize;
};

class CHUD
{
public:
	CHUD();
	~CHUD();

	void LoadHUD();

	void LoadArmourAndHealth(const CU::CJsonValue& aJsonValue);

	void Update(CU::Time aDeltaTime);
	void Render();

private:

	SHUDElement loadElement(const CU::CJsonValue& aJsonValue) const;

	void LoadText(const CU::CJsonValue& aJsonValue, CTextInstance& aTextInstance) const;
	CBarInstance* LoadBar(const CU::CJsonValue& aJsonValue);

	//health and armour
	SHUDElement myHealthAndArmourElement;
	CSpriteInstance* myHealthAndArmorSprite;

	CTextInstance myHealthNumber;
	CTextInstance myArmourNumber;

	CBarInstance* myHealthBar;
	CBarInstance* myArmourBar;

	bool myHealthAndArmourHasChanged;

	//weapon
	SHUDElement myWeaponElement;
	CSpriteInstance* myWeaponSprite;

	CTextInstance myAmmoNumber;

	//crosshair
	SHUDElement myCrosshairElement;
	CSpriteInstance* myCrosshairSprite;

	//test
	float testValue;
};

