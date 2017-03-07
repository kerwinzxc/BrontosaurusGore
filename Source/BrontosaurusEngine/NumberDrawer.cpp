#include "stdafx.h"
#include "NumberDrawer.h"
#include "Engine.h"
#include "CommonUtilities.h"


CNumberDrawer::CNumberDrawer() : myNumber(0)
{
	mySpriteInstances.Init(8);
	myCountingGoal = 0;
}


CNumberDrawer::~CNumberDrawer()
{
	mySpriteInstances.DeleteAll();
}

void CNumberDrawer::Init(const std::string& anImageFilePath, const CU::Vector2f& aSize, const CU::Vector2f& aPosition, const bool aHorizontalSprite)
{
	mySpriteData.myImageFilPath = anImageFilePath;
	mySpriteData.mySize = aSize;
	mySpriteData.myPosition = aPosition;
	mySpriteData.mySpriteIsHorizontal = aHorizontalSprite;

	AddSprite();
}

void CNumberDrawer::Render()
{
	for (char i = 0; i < mySpriteInstances.Size(); ++i)
	{
		mySpriteInstances[i]->Render();
	}
}

long CNumberDrawer::Increment()
{
	for (char i = mySpriteInstances.Size() - 1; i >= 0; --i)
	{

		CSpriteInstance* const currentSprite = mySpriteInstances[i];
		CU::Point4f currentRect = currentSprite->GetRect();

		currentRect.y -= 0.1f;
		currentRect.w -= 0.1f;

		if (currentRect.y > -0.01f)
		{
			currentSprite->SetRect(currentRect);
			break;
		}

		if (i == 0)
		{
			AddSprite();
			++i;
		}

		currentRect.y = 0.9f;
		currentRect.w = 1.f;

		currentSprite->SetRect(currentRect);
	}
	return ++myNumber;
}

long CNumberDrawer::Decrement()
{
	for (char i = mySpriteInstances.Size() - 1; i >= 0; --i)
	{
		CSpriteInstance& currentSprite = *mySpriteInstances[i];
		CU::Vector4f currentRect = currentSprite.GetRect();

		currentRect.y += 0.1f;
		currentRect.w += 0.1f;

		if (currentRect.w <= 1)
		{
			currentSprite.SetRect(currentRect);
		}

		if (i == 0)
		{
			RemoveSprite();
			break;
		}

		currentRect.y = 0.0f;
		currentRect.x = 0.1f;

		currentSprite.SetRect(currentRect);
	}
	return --myNumber;
}

void CNumberDrawer::Add(long aNumberToAdd)
{
	myCountingGoal += aNumberToAdd;
	if (myCountingGoal < 0)
	{
		myCountingGoal = 0;
	}
}

void CNumberDrawer::Update(int anIncreMentionSize)
{
	const int incrementationSize = MAX(anIncreMentionSize, 0);

	for (int i = 0; i < incrementationSize; ++i)
	{
		if (myNumber < myCountingGoal)
		{
			Increment();
		}
		else if(myNumber > myCountingGoal)
		{
			Decrement();
		}
		else
		{
			break;
		}
	}
}
