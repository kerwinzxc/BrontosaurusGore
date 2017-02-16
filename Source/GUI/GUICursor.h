#pragma once

#include "../PostMaster/Subscriber.h"

class CSpriteInstance;

class GUICursor : public Subscriber
{
public:
	GUICursor();
	~GUICursor();

	void Render();
	void SetPosition(const CU::Vector2f& aPosition);
	const CU::Vector2f& GetPosition() const;

	eMessageReturn Recieve(const Message& aMessage) override;

private:
	CSpriteInstance* mySprite;
	bool myHasRealPos;
};
