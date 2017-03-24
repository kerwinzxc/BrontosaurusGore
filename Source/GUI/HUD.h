#pragma once
#include "../BrontosaurusEngine/TextInstance.h"
#include "../CommonUtilities/JsonValue.h"

class CSpriteInstance;
class CBarInstance;

struct SHUDElement
{
	CU::Vector2f myScreenPosition;
	CU::Vector2f myScreenSize;
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
	void Render() const;

private:
	//health and armour
	SHUDElement myHealthAndArmour;
	CSpriteInstance* myHealthAndArmorSprite;

	CTextInstance myHealthNumber;
	CTextInstance myArmourNumber;

	CBarInstance* myHealthBar;
	CBarInstance* myArmourBar;

	//weapon
	SHUDElement myWeaponElement;
	CSpriteInstance* myWeaponSprite;

	CTextInstance myAmmoNumber;

	//crosshair
	SHUDElement myCrosshairElement;
	CSpriteInstance* myCrosshairSprite;
};

