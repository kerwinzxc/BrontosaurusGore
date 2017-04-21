#include "stdafx.h"
#include "MenuManager.h"
#include "BrontosaurusEngine/RenderMessages.h"
#include "BrontosaurusEngine/Renderer.h"
#include "BrontosaurusEngine/Engine.h"
#include "JsonValue.h"
#include "..\Audio\AudioInterface.h"

CU::Vector2f CMenuManager::ourMousePosition(0.5f, 0.5f);

bool CompareLayers(SLayerData aFirstData, SLayerData aSecondData)
{
	return aFirstData.myLayer > aSecondData.myLayer;
}

CMenuManager::CMenuManager() : myPointerSprite(nullptr), myShouldRender(true), myCurentlyHoveredClickarea(-1), myMouseIsPressed(false)
{
	myClickAreas.Init(1);
	mySpriteInstances.Init(1);
	myTextInstances.Init(1);
	myLayers.Init(1);

	myGUIElement.myOrigin = CU::Vector2f(0.5f, 0.5f);
	myGUIElement.myScreenRect = CU::Vector4f(0.5f, 0.5f, 1.5f, 1.5f);
	myHasPlayedHoverSound = false;
	myHasPlayedClickSound = false;
}


CMenuManager::~CMenuManager()
{
}

void CMenuManager::CreateClickArea(CU::GrowingArray<std::string> someActions, CU::GrowingArray<std::string> someArguments, const int aSpriteID, CU::Vector4f aRect, const unsigned char aLayer)
{
	SClickArea clickArea;
	clickArea.myRect = aRect;
	clickArea.mySpriteID = aSpriteID;
	clickArea.myActions.Init(1);

	for (int i = 0; i < someActions.Size(); ++i)
	{
		clickArea.myActions.Add(bind(myActions[someActions[i]], someArguments[i]));
	}

	myClickAreas.Add(clickArea);

	myLayers.Add({ aLayer, myClickAreas.Size() - 1, eMenuThingType::eClickArea });
}

int CMenuManager::CreateSprite(const std::string& aFolder, const CU::Vector2f aPosition, const CU::Vector2f anOrigin, const unsigned char aLayer)
{
	SMenuSprite menuSprite;

	if (CU::CJsonValue::FileExists(aFolder + "/" + "default.dds"))
	{
		menuSprite.myDafaultSprite = new CSpriteInstance((aFolder + "/" + "default.dds").c_str());
		menuSprite.myDafaultSprite->SetPosition(aPosition);
		menuSprite.myDafaultSprite->SetPivot(anOrigin);
	}
	else
	{
		DL_PRINT_WARNING("no default menu immage found no sprite created");
		return -1;
	}

	if (CU::CJsonValue::FileExists(aFolder + "/" + "onHover.dds"))
	{
		menuSprite.myOnHoverSprite = new CSpriteInstance((aFolder + "/" + "onHover.dds").c_str());
		menuSprite.myOnHoverSprite->SetPosition(aPosition);
		menuSprite.myOnHoverSprite->SetPivot(anOrigin);
	}
	else
	{
		menuSprite.myOnHoverSprite = menuSprite.myDafaultSprite;
		DL_PRINT("on hover sprite missing using default instead");
	}

	if (CU::CJsonValue::FileExists(aFolder + "/" + "onClick.dds"))
	{
		menuSprite.myOnClickSprite = new CSpriteInstance((aFolder + "/" + "onClick.dds").c_str());
		menuSprite.myOnClickSprite->SetPosition(aPosition);
		menuSprite.myOnClickSprite->SetPivot(anOrigin);
	}
	else
	{
		menuSprite.myOnClickSprite = menuSprite.myDafaultSprite;
		DL_PRINT("on click sprite missing using default instead");
	}

	if (CU::CJsonValue::FileExists(aFolder + "/" + "inactive.dds"))
	{
		menuSprite.myInactiveSprite = new CSpriteInstance((aFolder + "/" + "inactive.dds").c_str());
		menuSprite.myInactiveSprite->SetPosition(aPosition);
		menuSprite.myInactiveSprite->SetPivot(anOrigin);
	}
	else
	{
		menuSprite.myInactiveSprite = menuSprite.myDafaultSprite;
		DL_PRINT("inactive sprite missing using default instead");
	}

	mySpriteInstances.Add(menuSprite);

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
	myLayers.Add({ aLayer, myTextInstances.Size() - 1, eMenuThingType::eText });
	return myTextInstances.Size() - 1;
}

void CMenuManager::SetMousePointer(CSpriteInstance* aMousePointer)
{
	if (aMousePointer != nullptr)
	{
		delete myPointerSprite;
		myPointerSprite = nullptr;
	}

	myPointerSprite = aMousePointer;
	myPointerSprite->SetPosition(ourMousePosition);
}

void CMenuManager::Update(const CU::Time& aDeltaTime)
{
	bool hasCollided = false;
	for (unsigned i = 0; i < myClickAreas.Size(); ++i)
	{
		CU::Vector2f lowerRight;
		CU::Vector2f upperLeft;

		if (myClickAreas.At(i).mySpriteID >= 0)
		{
			CSpriteInstance* currentSprite = mySpriteInstances[myClickAreas[i].mySpriteID].myDafaultSprite;
			lowerRight = currentSprite->GetPosition() + currentSprite->GetSize() * myClickAreas[i].myRect.zw - currentSprite->GetPivot() * currentSprite->GetSize();
			upperLeft = currentSprite->GetPosition() + currentSprite->GetSize() * myClickAreas[i].myRect.xy - currentSprite->GetPivot() * currentSprite->GetSize();
		}
		else
		{
			upperLeft = myClickAreas[i].myRect.xy;
			lowerRight = myClickAreas[i].myRect.zw;
		}

		if (ourMousePosition.x > upperLeft.x && ourMousePosition.y > upperLeft.y && ourMousePosition.x < lowerRight.x && ourMousePosition.y < lowerRight.y)
		{
			if (myCurentlyHoveredClickarea != i)
			{
				if (myCurentlyHoveredClickarea > -1 && myClickAreas[myCurentlyHoveredClickarea].mySpriteID >= 0)
				{
					mySpriteInstances[myClickAreas[myCurentlyHoveredClickarea].mySpriteID].myState = eMenuButtonState::eDefault;
				}
				myCurentlyHoveredClickarea = i;
			}

			if (myMouseIsPressed == true && myClickAreas[myCurentlyHoveredClickarea].mySpriteID >= 0 && mySpriteInstances[myClickAreas[i].mySpriteID].myState != eMenuButtonState::eOnClick)
			{
				if (myHasPlayedClickSound == false)
				{
					Audio::CAudioInterface::GetInstance()->PostEvent("Menu_Click");
					myHasPlayedClickSound = true;
				}
			
				mySpriteInstances[myClickAreas[i].mySpriteID].myState = eMenuButtonState::eOnClick;
			}
			else if (myClickAreas[i].mySpriteID >= 0)
			{
				mySpriteInstances[myClickAreas[i].mySpriteID].myState = eMenuButtonState::eOnHover;
			}

			hasCollided = true;
			break;
		}
	}



	if (hasCollided == false && myCurentlyHoveredClickarea > -1)
	{
		if (myClickAreas[myCurentlyHoveredClickarea].mySpriteID >= 0)
		{
			mySpriteInstances[myClickAreas[myCurentlyHoveredClickarea].mySpriteID].myState = eMenuButtonState::eDefault;
			myHasPlayedHoverSound = false;
			myHasPlayedClickSound = false;
		}
		myCurentlyHoveredClickarea = -1;
	}

	if (hasCollided == true && myHasPlayedHoverSound == false)
	{
		Audio::CAudioInterface::GetInstance()->PostEvent("Menu_Hover");
		myHasPlayedHoverSound = true;
	}
}

void CMenuManager::Render()
{
	if (myShouldRender == true)
	{
		SCreateOrClearGuiElement* elementMessage = new SCreateOrClearGuiElement(L"__Menu", myGUIElement, CU::Vector2ui(1920, 1080));
		RENDERER.AddRenderMessage(elementMessage);

		SChangeStatesMessage* const changeStatesMessage = new SChangeStatesMessage();
		changeStatesMessage->myBlendState = eBlendState::eAlphaBlend;
		changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
		changeStatesMessage->mySamplerState = eSamplerState::eClamp;

		SRenderToGUI*const guiChangeState = new SRenderToGUI(L"__Menu", changeStatesMessage);
		RENDERER.AddRenderMessage(guiChangeState);

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
				CSpriteInstance *spriteInstance = ChoseSpriteInstance(mySpriteInstances.At(layeredThing.myIndex));
				spriteInstance->RenderToGUI(L"__Menu");
			}
			break;
			case eMenuThingType::eClickArea:
			default: break;
			}
		}

		if (myPointerSprite != nullptr)
		{
			myPointerSprite->RenderToGUI(L"__Menu");
		}
	}


}

void CMenuManager::MousePressed()
{
	myMouseIsPressed = true;
}

void CMenuManager::MouseReleased()
{
	myMouseIsPressed = false;

	if (myCurentlyHoveredClickarea >= 0)
	{
		for (int i = 0; i < myClickAreas[myCurentlyHoveredClickarea].myActions.Size(); ++i)
		{
			if (myClickAreas[myCurentlyHoveredClickarea].myActions[i]() == false)
			{
				break;
			}
		}
	}
}

const SMenuSprite& CMenuManager::GetSprite(unsigned aSpriteId)
{
	return mySpriteInstances.At(aSpriteId);
}

void CMenuManager::AddAction(const std::string &aActionName, const std::function<bool(std::string)> &aFunction)
{
	myActions[aActionName] = aFunction;
}

CTextInstance* CMenuManager::GetTextInstance(const int aTextInputTextInstanceIndex)
{
	return myTextInstances[aTextInputTextInstanceIndex];
}

CSpriteInstance* CMenuManager::ChoseSpriteInstance(const SMenuSprite& aMenuSprite)
{
	switch (aMenuSprite.myState)
	{
	case eMenuButtonState::eDefault:
		return aMenuSprite.myDafaultSprite;
	case eMenuButtonState::eOnHover:
		return  aMenuSprite.myOnHoverSprite;
	case eMenuButtonState::eOnClick:
		return  aMenuSprite.myOnClickSprite;
	case eMenuButtonState::eInnactive:
		return aMenuSprite.myInactiveSprite;
	default:
		return aMenuSprite.myDafaultSprite;
	}
}
