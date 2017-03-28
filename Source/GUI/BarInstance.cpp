#include "stdafx.h"
#include "BarInstance.h"
#include "BrontosaurusEngine/RenderMessages.h"
#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/Renderer.h"


CBarInstance::CBarInstance(const CU::Colour &aFullColour, const CU::Vector4f& aRect): myCurrentLevel(1), myFullColour(aFullColour), myEmptyColour(aFullColour), myRect(aRect)
{
}

CBarInstance::CBarInstance(const CU::Colour& aBackgroundColour, const CU::Colour& aFullColour, const CU::Colour& anEmptyColour, const CU::Vector4f& aRect): 
myCurrentLevel(0), 
myFullColour(aFullColour), 
myBackgroundColour(aBackgroundColour),
myRect(aRect),
myEmptyColour(anEmptyColour)
{

}

CBarInstance::~CBarInstance()
{
}

void CBarInstance::Update(CU::Time aDeltaTime)
{
}

void CBarInstance::Render() const
{
	SRenderBarMessage *renderBarMessage = new SRenderBarMessage;
	renderBarMessage->myFullColour = myFullColour;
	renderBarMessage->myBackgroundColour = myBackgroundColour;
	renderBarMessage->myCurrentLevel = myCurrentLevel;
	renderBarMessage->myRect = myRect;
	renderBarMessage->myEmptyColour = myEmptyColour;

	RENDERER.AddRenderMessage(renderBarMessage);
}

void CBarInstance::RenderToGUI(const std::wstring& aStr) const
{
	SRenderBarMessage *renderBarMessage = new SRenderBarMessage;
	renderBarMessage->myFullColour = myFullColour;
	renderBarMessage->myBackgroundColour = myBackgroundColour;
	renderBarMessage->myCurrentLevel = myCurrentLevel;
	renderBarMessage->myRect = myRect;
	renderBarMessage->myEmptyColour = myEmptyColour;

	SRenderToGUI * renderToGuiMessage = new SRenderToGUI(aStr, renderBarMessage);
	RENDERER.AddRenderMessage(renderToGuiMessage);
}

float CBarInstance::GetLevel() const
{
	return myCurrentLevel;
}

void CBarInstance::SetLevel(const float aLevel)
{
	myCurrentLevel = aLevel;
}



