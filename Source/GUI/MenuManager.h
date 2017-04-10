#pragma once
#include "BrontosaurusEngine/SpriteInstance.h"
#include "BrontosaurusEngine/TextInstance.h"
#include "GUIElement.h"


enum class eMenuThingType
{
	eClickArea,
	eText,
	eSprite
};


struct SClickArea
{
	unsigned mySpriteID;
	unsigned myCallbackID;
};

struct SLayerData
{
	unsigned char myLayer;
	unsigned myIndex;
	eMenuThingType myMenuThing;
};

class CMenuManager
{
public:
	CMenuManager();
	~CMenuManager();

	void CreateClickArea(const unsigned aCalbackID, const unsigned aSpriteID, const unsigned char aLayer);
	unsigned CreateSprite(CSpriteInstance* aSpriteInstance, const unsigned char aLayer);
	unsigned CreateText(const std::string& aFontName, const CU::Vector2f& aPosition, const std::wstring someText, const unsigned char aLayer, const eAlignment anAlignment = eAlignment::eLeft);

	void Update(const CU::Time& aDeltaTime);
	void Render();

	void UpdateMousePosition(const CU::Vector2f& aPosition);
private:
	CU::GrowingArray<CSpriteInstance*> mySpriteInstances;
	CU::GrowingArray<CTextInstance*> myTextInstances;
	CU::GrowingArray<SClickArea> myClickAreas;
	CU::GrowingArray<SLayerData> myLayers;

	CU::Vector2f myMousePosition;

	bool myShouldRender;

	SGUIElement myGUIElement;
};

inline void CMenuManager::UpdateMousePosition(const CU::Vector2f& aPosition)
{
	myMousePosition = aPosition;
}
