#include "stdafx.h"
#include "HUD.h"
#include "JsonValue.h"
#include "BrontosaurusEngine/SpriteInstance.h"
#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/Renderer.h"


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
	const std::string backgroundPath = aJsonValue.at("background").GetString();
	myHealthAndArmorSprite = new CSpriteInstance(backgroundPath.c_str(), CU::Vector2f(1.f, 1.f));
	
}

void CHUD::Update(CU::Time aDeltaTime)
{

}

void CHUD::Render() const
{
	/*SGUIElement tempElement;
	tempElement.myScreenRect = CU::Vector4f(0.2f, 0.2f, 0.1f, 0.2f);
	tempElement.myAnchor = 0;
	tempElement.myOrigin = CU::Vector2f(0.f, 0.f);

	RENDERER.AddRenderMessage(new SCreateOrClearGuiElement(L"TestElement", tempElement, CU::Vector2ui(256, 64)));

	myHealthAndArmorSprite->RenderToGUI(L"TestElement");*/
}
