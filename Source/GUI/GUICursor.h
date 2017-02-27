#pragma once

#include "../PostMaster/Subscriber.h"
#include "ThreadedPostmaster/Subscriber.h"

class CSpriteInstance;

class GUICursor : public Postmaster::ISubscriber
{
public:
	GUICursor();
	~GUICursor();

	void Render();
	void SetPosition(const CU::Vector2f& aPosition);
	const CU::Vector2f& GetPosition() const;


private:
	CSpriteInstance* mySprite;
	bool myHasRealPos;
};
