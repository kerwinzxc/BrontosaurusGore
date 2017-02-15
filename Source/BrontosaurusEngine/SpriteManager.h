#pragma once
//#include <GrowingArray.h>
#include <map>
#include <mutex>

class CSprite;

class CSpriteManager
{
public:
	using SpriteID = int;
	friend class CEngine;

	CSprite* CreateSprite(const char* aSpritePath);
	void DestroySprite(CSprite* aSprite);

private:
	CSpriteManager();
	~CSpriteManager();

private:
	std::map<std::string, SpriteID> mySpriteIDs;
	CU::GrowingArray<CSprite*, SpriteID> mySprites;
	std::mutex myLoadMutex;
	static const SpriteID ourMaxNumberOfSprites = 512;
};

