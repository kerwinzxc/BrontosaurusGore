#pragma once
#include "BrontosaurusEngine/SpriteInstance.h"

class CMenuSprite
{
public:
	CMenuSprite();
	~CMenuSprite();

	virtual void Update(const CU::Time& aDeltaTime) = 0;
	virtual void Render() = 0;

private:
	//CU::Vector2f myPosition;
	//CSpriteInstance mySprite;
	//std::wstring myElementName;
};

