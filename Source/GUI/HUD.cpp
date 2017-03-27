#include "stdafx.h"
#include "HUD.h"
#include "JsonValue.h"
#include "BrontosaurusEngine/SpriteInstance.h"
#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/Renderer.h"
#include "CommonUtilities.h"
#include "BarInstance.h"


CHUD::CHUD() : myHealthAndArmorSprite(nullptr), myHealthBar(nullptr), myArmourBar(nullptr), myWeaponSprite(nullptr), myCrosshairSprite(nullptr), testValue(0.f)
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
	myHealthBar = new CBarInstance(CU::Colour(0, 1, 0, 1), CU::Vector4f(1, 0, 0, 1), CU::Vector4f(0, 0, 1, 1));
}

void CHUD::Update(CU::Time aDeltaTime)
{
	myHealthBar->SetLevel(MAX(myHealthBar->GetLevel() - 0.01, 0));
}

void CHUD::Render() const
{
	SGUIElement tempElement;
	tempElement.myScreenRect = CU::Vector4f(0.2f, 0.2f, 0.1f, 0.2f);
	tempElement.myAnchor = 0;
	tempElement.myOrigin = CU::Vector2f(0.f, 0.f);

	RENDERER.AddRenderMessage(new SCreateOrClearGuiElement(L"TestElement", tempElement, CU::Vector2ui(256, 64)));

	myHealthBar->RenderToGUI(L"TestElement");
}
