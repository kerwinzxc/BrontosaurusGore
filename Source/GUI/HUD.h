#pragma once
#include "../BrontosaurusEngine/TextInstance.h"
#include "../CommonUtilities/JsonValue.h"
#include "GUIElement.h"
#include "map"

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
	void LoadWeaponHud(const CU::CJsonValue& aJsonValue);
	void LoadCrosshair(const CU::CJsonValue& aJsonValue);
	void UpdateHealthAndArmour();
	void UpdateWeapon();
	void Update(CU::Time aDeltaTime);
	void SetAmmoHudRect();
	void Render();

private:

	SHUDElement LoadElement(const CU::CJsonValue& aJsonValue) const;

	void LoadText(const CU::CJsonValue& aJsonValue, CTextInstance& aTextInstance) const;
	CBarInstance* LoadBar(const CU::CJsonValue& aJsonValue);

	//health and armour
	SHUDElement myHealthAndArmourElement;
	CSpriteInstance* myHealthAndArmorSprite;

	CTextInstance myHealthNumber;
	int myCurrentHealth;
	int myCurrentMaxHealth;

	CTextInstance myArmourNumber;
	int myCurrentArmour;
	int myCurrentMaxArmour;

	CBarInstance* myHealthBar;
	CBarInstance* myArmourBar;

	volatile bool myHealthAndArmourHasChanged;

	//weapon
	SHUDElement myWeaponElement;
	CSpriteInstance* myWeaponSprite;

	CTextInstance myAmmoNumber;

	char myCurrentWeapon; 
	char myTransitionLength;
	volatile bool myWeaponHUDHasChanged;
	std::map<std::string, char> myWeaponIndexes;
	CU::CBitSet<3> myPickedUpWeapons;

	CU::Vector2f myFrameSize;
	unsigned short myCurrentAmmo;
	unsigned short myCurrentMaxAmmo;

	unsigned myAmmountOfWeapons;
	//crosshair
	SHUDElement myCrosshairElement;
	CSpriteInstance* myCrosshairSprite;
	volatile bool myCrosshairHasUpdated;

};

