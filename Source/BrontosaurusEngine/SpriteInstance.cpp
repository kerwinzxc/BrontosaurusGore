#include "stdafx.h"
#include "SpriteInstance.h"
#include "Sprite.h"
#include "Engine.h"
#include "Renderer.h"
#include "SpriteManager.h"

CSpriteInstance::CSpriteInstance(const char* aTexturePath)
	: mySprite(nullptr)
	, myRect(0.f, 0.f, 1.f, 1.f)
	, myColor(1.f, 1.f, 1.f, 1.f)
	, myRotation(0)
{
	mySprite = SPRMGR.CreateSprite(aTexturePath);
	CU::Vector2f windowSize(WINDOW_SIZE);
	CU::Vector2f textureSize(mySprite->GetTextureSizeFloat());
	CU::Vector2f normalizedSize = textureSize / windowSize;
	mySize = normalizedSize;
}

CSpriteInstance::CSpriteInstance(const char* aTexturePath, const CU::Vector2f& aSize, const CU::Vector2f& aPosition, const CU::Vector2f& aPivot, const CU::Vector4f& aRect, const CU::Vector4f& aColour, const bool aShouldAutoSize)
	: myPosition(aPosition)
	, mySize(aSize)
	, myPivot(aPivot)
	, myRect(aRect)
	, myColor(aColour)
	, mySprite(nullptr)
	, myRotation(0)
{
	mySprite = SPRMGR.CreateSprite(aTexturePath);

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
	renderMessage->myRotation = myRotation;

	RENDERER.AddRenderMessage(renderMessage);
}

void CSpriteInstance::RenderToGUI(const std::wstring& anElementName)
{
	SRenderSpriteMessage* renderMessage = new SRenderSpriteMessage();
	renderMessage->myPosition = myPosition;
	renderMessage->mySize = mySize;
	renderMessage->myRect = myRect;
	renderMessage->myPivot = myPivot;
	renderMessage->myRotation = myRotation;
	renderMessage->myColor = myColor;
	renderMessage->mySprite = mySprite;

	SRenderToGUI* renderToGuiMessage = new SRenderToGUI(anElementName, renderMessage);

	RENDERER.AddRenderMessage(renderToGuiMessage);
}

CU::Vector2f CSpriteInstance::GetTextureSize() const
{
	if (mySprite != nullptr)
	{
		return mySprite->GetTextureSizeFloat();
	}

	return CU::Vector2f::Zero;
}

void CSpriteInstance::SetRotation(float aRotation)
{
	myRotation = aRotation;
}

void CSpriteInstance::Rotate(float aRotationAmmount)
{
	myRotation += aRotationAmmount;
}
