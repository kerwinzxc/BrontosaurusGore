#include "stdafx.h"
#include "SpriteManager.h"
#include "Sprite.h"


CSpriteManager::CSpriteManager()
	: mySprites(ourMaxNumberOfSprites)
{
}


CSpriteManager::~CSpriteManager()
{
	mySpriteIDs.clear();
	mySprites.DeleteAll();
}

CSprite* CSpriteManager::CreateSprite(const char* aSpritePath)
{
	std::lock_guard<std::mutex> mutexGuard(myLoadMutex);
	//myLoadMutex.lock();

	if (mySpriteIDs.find(aSpritePath) == mySpriteIDs.end())
	{
		SpriteID spriteID = mySprites.Size();
		CSprite* newSprite = new CSprite();
		newSprite->Init(aSpritePath);
		mySprites.Add(newSprite);
		mySpriteIDs[aSpritePath] = spriteID;
	}

	SpriteID spriteID = mySpriteIDs[aSpritePath];
	//myLoadMutex.unlock();
	return mySprites[spriteID];
}

void CSpriteManager::DestroySprite(CSprite* /*aSprite*/)
{
	//SpriteIndex index = mySprites.Find(aSprite);
	//if (index != mySprites.FoundNone)
	//{
	//	mySprites.RemoveCyclicAtIndex(index);
	//}
}
