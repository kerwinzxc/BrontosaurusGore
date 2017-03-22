#include "stdafx.h"
#include "BarInstance.h"
#include "BrontosaurusEngine/RenderMessages.h"
#include "BrontosaurusEngine/Engine.h"
#include "BrontosaurusEngine/Renderer.h"


CBarInstance::CBarInstance(const CU::Colour &aFullColour, const CU::Vector4f& aRect): myCurrentLevel(1), myFullColour(aFullColour), myRect(aRect), myInterpolationData(nullptr)
{
}

CBarInstance::CBarInstance(const CU::Colour& aFullColour, const CU::Colour& anEmptyColour, const CU::Vector4f& aRect, const eBarInterMode aInterMode):
	myCurrentLevel(1),
	myRect(aRect),
	myFullColour(aFullColour),
	myInterpolationData(new SBarInterpolationData)
{
	myInterpolationData->myEmptyColour = anEmptyColour;
	myInterpolationData->myInterMode = aInterMode;
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

	if (myInterpolationData != nullptr)
	{
		renderBarMessage->myEmptyColour = myInterpolationData->myEmptyColour;
	}
	else
	{
		renderBarMessage->myEmptyColour = myFullColour;
	}

	RENDERER.AddRenderMessage(renderBarMessage);
}

void CBarInstance::SetLevel(const float aLevel)
{
	myCurrentLevel = aLevel;
}
