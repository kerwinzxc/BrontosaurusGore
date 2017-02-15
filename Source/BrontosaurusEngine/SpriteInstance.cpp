#include "stdafx.h"
#include "SpriteInstance.h"
#include "Sprite.h"
#include "Engine.h"
#include "Renderer.h"
#include "SpriteManager.h"

CSpriteInstance::CSpriteInstance(const char* aTexturePath, const CU::Vector2f& aSize, const CU::Vector2f& aPosition, const CU::Vector2f& aPivot, const CU::Vector4f& aRect, const CU::Vector4f& aColour, const bool aShouldAutoSize)
	: myPosition(aPosition), mySize(aSize),
	myPivot(aPivot), myRect(aRect),
	myColor(aColour), mySprite(nullptr)
{
	mySprite = SPRMGR.CreateSprite(aTexturePath);
	//mySprite = new CSprite();
	//mySprite->Init(aTexturePath);

	if (aShouldAutoSize == true)
	{
		mySize.Set(mySprite->GetTextureSizeFloat().x / WINDOW_SIZE.x, mySprite->GetTextureSizeFloat().y / WINDOW_SIZE.y);
	}
}

CSpriteInstance::~CSpriteInstance()
{
	//SAFE_DELETE(mySprite);
	SPRMGR.DestroySprite(mySprite);
	mySprite = nullptr;
}

void CSpriteInstance::Render()
{
	SRenderSpriteMessage* renderMessage = new SRenderSpriteMessage();
	renderMessage->myPosition = myPosition;
	renderMessage->mySize = mySize;
	renderMessage->myRect = myRect;
	renderMessage->myPivot = myPivot;
	renderMessage->myColor = myColor;
	renderMessage->mySprite = mySprite;

	RENDERER.AddRenderMessage(renderMessage);
	//mySprite->Render(myPosition, mySize);
}

CU::Vector2f CSpriteInstance::GetTextureSize() const
{
	if (mySprite != nullptr)
	{
		return mySprite->GetTextureSizeFloat();
	}

	return CU::Vector2f::Zero;
}
