#include "stdafx.h"
#include "MenuManager.h"
#include "BrontosaurusEngine/RenderMessages.h"
#include "BrontosaurusEngine/Renderer.h"
#include "BrontosaurusEngine/Engine.h"

bool CompareLayers(SLayerData aFirstData, SLayerData aSecondData)
{
	return aFirstData.myLayer > aSecondData.myLayer;
}

CMenuManager::CMenuManager(): myShouldRender(true)
{
	myClickAreas.Init(1);
	mySpriteInstances.Init(1);
	myTextInstances.Init(1);
	myLayers.Init(1);
}


CMenuManager::~CMenuManager()
{
}

void CMenuManager::CreateClickArea(const unsigned aCalbackID, const unsigned aSpriteID, const unsigned char aLayer)
{
	myClickAreas.Add({ aCalbackID, aSpriteID });

	myLayers.Add({ aLayer, myClickAreas.Size() - 1, eMenuThingType::eClickArea });
}

unsigned CMenuManager::CreateSprite(CSpriteInstance* aSpriteInstance, const unsigned char aLayer)
{
	CSpriteInstance* newSprite = aSpriteInstance;
	mySpriteInstances.Add(newSprite);

	myShouldRender = true;
	myLayers.Add({ aLayer, mySpriteInstances.Size() - 1, eMenuThingType::eSprite });
	return mySpriteInstances.Size() - 1;
}

unsigned CMenuManager::CreateText(const std::string& aFontName, const CU::Vector2f& aPosition, const std::wstring someText, const unsigned char aLayer, const eAlignment anAlignment)
{
	CTextInstance* newText = new CTextInstance();
	newText->Init(aFontName);
	newText->SetPosition(aPosition);
	newText->SetText(someText);
	newText->SetAlignment(anAlignment);

	myTextInstances.Add(newText);

	myShouldRender = true;
	myLayers.Add({ aLayer, mySpriteInstances.Size() - 1, eMenuThingType::eText });
	return myTextInstances.Size() - 1;
}

void CMenuManager::Update(const CU::Time& aDeltaTime)
{
	for (unsigned i = 0; i < myClickAreas.Size(); ++i)
	{
		CSpriteInstance* currentSprite = mySpriteInstances[myClickAreas[i].mySpriteID];
		const CU::Vector2f lowerRight = currentSprite->GetPosition() + currentSprite->GetSize();
		if (myMousePosition.x > currentSprite->GetPosition().x && myMousePosition.y > currentSprite->GetPosition().y && myMousePosition.x < lowerRight.x && myMousePosition.y < lowerRight.y)
		{
			std::wstring out;
			out += L"Mouse is in click area ";
			out += std::to_wstring(i);

			DL_PRINT(out.c_str());
			break;
		}
	}
}

void CMenuManager::Render()
{
	if (myShouldRender == true)
	{
		SCreateOrClearGuiElement* elementMessage = new SCreateOrClearGuiElement(L"__Menu", myGUIElement, CU::Vector2ui(1920 , 1080));
		RENDERER.AddRenderMessage(elementMessage);

		myLayers.QuickSort(&CompareLayers);

		for (unsigned i = 0; i < myLayers.Size(); ++i)
		{
			SLayerData &layeredThing = myLayers.At(i);

			switch (layeredThing.myMenuThing)
			{
			case eMenuThingType::eText:
				{
					CTextInstance * currentTextInstance = myTextInstances.At(layeredThing.myIndex);
					currentTextInstance->RenderToGUI(L"__Menu");
				}
				break;
			case eMenuThingType::eSprite:
				{
					CSpriteInstance *spriteInstance = mySpriteInstances.At(layeredThing.myIndex);
					spriteInstance->RenderToGUI(L"__Menu");
				}
				break;
			case eMenuThingType::eClickArea:
			default: break;
			}
		}
	}
}
