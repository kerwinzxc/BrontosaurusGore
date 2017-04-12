#pragma once
#include "BrontosaurusEngine/SpriteInstance.h"
#include "BrontosaurusEngine/TextInstance.h"
#include "GUIElement.h"
#include "CommonUtilities.h"


enum class eMenuThingType
{
	eClickArea,
	eText,
	eSprite
};


enum class eMenuButtonState
{
	eDefault,
	eOnHover,
	eOnClick,
	eInnactive
};

struct SClickArea
{
	int mySpriteID;
	CU::Vector4f myRect;
	CU::GrowingArray<std::function<void(void)>> myActions;
};

struct SLayerData
{
	unsigned char myLayer;
	unsigned myIndex;
	eMenuThingType myMenuThing;
};

struct SMenuSprite
{
	SMenuSprite(): myState(eMenuButtonState::eDefault), myDafaultSprite(nullptr), myOnHoverSprite(nullptr), myOnClickSprite(nullptr), myInactiveSprite(nullptr)
	{
	}

	eMenuButtonState myState;
	CSpriteInstance* myDafaultSprite;
	CSpriteInstance* myOnHoverSprite;
	CSpriteInstance* myOnClickSprite;
	CSpriteInstance* myInactiveSprite;
};

class CMenuManager
{
public:
	CMenuManager();
	~CMenuManager();

	void CreateClickArea(CU::GrowingArray<std::string> someActions, CU::GrowingArray<std::string> someArguments, const unsigned aSpriteID, CU::Vector4f aRect, const unsigned char aLayer);
	int CreateSprite(const std::string& aFolder, const CU::Vector2f aPosition, const CU::Vector2f anOrigin, const unsigned char aLayer);
	unsigned CreateText(const std::string& aFontName, const CU::Vector2f& aPosition, const std::wstring someText, const unsigned char aLayer, const eAlignment anAlignment = eAlignment::eLeft);

	void SetMousePointer(CSpriteInstance* aMousePointer);

	void Update(const CU::Time& aDeltaTime);
	void Render();

	void UpdateMousePosition(const CU::Vector2f& aPosition);
	CU::Vector2f GetMopusePosition() const;
	void MousePressed();
	void MouseReleased();

	const SMenuSprite& GetSprite(unsigned aSpriteId);

	void AddAction(const std::string& aActionName, const std::function<void(std::string)>& aFunction);
private:
	static CSpriteInstance* ChoseSpriteInstance(const SMenuSprite& aMenuSprite);

	std::map<std::string, std::function<void(std::string)>> myActions;

	CU::GrowingArray<SMenuSprite> mySpriteInstances;
	CU::GrowingArray<CTextInstance*> myTextInstances;
	CU::GrowingArray<SClickArea> myClickAreas;
	CU::GrowingArray<SLayerData> myLayers;

	CU::Vector2f myMousePosition;
	CSpriteInstance* myPointerSprite;

	bool myShouldRender;

	SGUIElement myGUIElement;
	int myCurentlyHoveredClickarea;
	bool myMouseIsPressed;
};

inline void CMenuManager::UpdateMousePosition(const CU::Vector2f& aPosition)
{
	myMousePosition = aPosition;

	myMousePosition.x = CLAMP(myMousePosition.x, 0, 1);
	myMousePosition.y = CLAMP(myMousePosition.y, 0, 1);

	if (myPointerSprite != nullptr)
	{
		myPointerSprite->SetPosition(myMousePosition);
	}
}

inline CU::Vector2f CMenuManager::GetMopusePosition() const
{
	return myMousePosition;
}
