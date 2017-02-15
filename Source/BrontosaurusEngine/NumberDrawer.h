#pragma once
#include "SpriteInstance.h"
#include "DynamicString.h"

namespace
{
	const float digitOffset = 0.65f;
}

class CNumberDrawer
{
public:
	CNumberDrawer();
	~CNumberDrawer();

	void Init(const CU::DynamicString& anImageFilePath, const CU::Vector2f& aSize, const CU::Vector2f& aPosition, const bool aHorizontalSprite = false);
	void Render();

	long GetNumber()const;
	long Increment();
	long Decrement();

	void Add(long aNumberToAdd);
	void Update(int anIncreMentionSize = 1);

private:
	void AddSprite();
	void RemoveSprite();

	struct
	{
		CU::DynamicString myImageFilPath;
		CU::Vector2f mySize;
		CU::Point2f myPosition;
		bool mySpriteIsHorizontal;
	}mySpriteData;

	long myCountingGoal;
	long myNumber;

	CU::GrowingArray<CSpriteInstance*, char> mySpriteInstances;
};

inline long CNumberDrawer::GetNumber() const
{
	return myNumber;
}


inline void CNumberDrawer::AddSprite()
{
	for (char i = 0; i < mySpriteInstances.Size(); ++i)
	{
		CU::Vector2f position = mySpriteInstances[i]->GetPosition();
		position.x += mySpriteData.mySize.x * digitOffset;
		mySpriteInstances[i]->SetPosition(position);
	}
	CSpriteInstance* aPointer = new CSpriteInstance(mySpriteData.myImageFilPath.c_str(), mySpriteData.mySize, { 0.f,0.f }, { mySpriteData.myPosition.x , mySpriteData.myPosition.y }, { 0, 0.9f, 1,1.f });
	mySpriteInstances.Insert(0 , aPointer);
}

inline void CNumberDrawer::RemoveSprite()
{
	for (char i = 0; i < mySpriteInstances.Size(); ++i)
	{
		CU::Point2f position = mySpriteInstances[i]->GetPosition();
		position.x -= mySpriteData.mySize.x * digitOffset;
		mySpriteInstances[i]->SetPosition(position);
	}

	delete mySpriteInstances[0];
	mySpriteInstances[0] = nullptr;

	mySpriteInstances.RemoveAtIndex(0);
}
