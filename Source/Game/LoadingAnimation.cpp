#include "stdafx.h"
#include "LoadingAnimation.h"
#include "CUTime.h"
#include "SpriteInstance.h"


CLoadingAnimation::CLoadingAnimation(): mySpriteInstance(nullptr)
{
}


CLoadingAnimation::~CLoadingAnimation()
{
	delete mySpriteInstance;
}

void CLoadingAnimation::Init(CSpriteInstance* aSpriteInstance)
{
	mySpriteInstance = aSpriteInstance;
	myPosition = CU::Vector2f(-0.1, 0.5);
}

void CLoadingAnimation::Update(const CU::Time& aDeltatime)
{
	myPosition += CU::Vector2f(0.5, 0) * aDeltatime.GetSeconds();
	if (myPosition.x > 1)
	{
		myPosition.x = -0.1f;
	}
}

void CLoadingAnimation::Render()
{
	mySpriteInstance->SetPosition(myPosition);
	mySpriteInstance->Render();
}
