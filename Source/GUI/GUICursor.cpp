#include "stdafx.h"
#include "GUICursor.h"
#include "../BrontosaurusEngine/SpriteInstance.h"
#include "../BrontosaurusEngine/Engine.h"

#include "../PostMaster/Event.h"
#include "../PostMaster/Message.h"

GUICursor::GUICursor()
	: mySprite(nullptr)
	, myHasRealPos(false)
{
	mySprite = new CSpriteInstance("Models/mousePointer/mousePointer.dds", CU::Vector2f::Zero, CU::Vector2f(0.5f, 0.5f), { 0.f, 0.f }, { 0.f, 0.f, 1.f, 1.f }, { 1.f, 1.f ,1.f ,1.f }, true); //TODO: FIX hardcoded windowsize
}

GUICursor::~GUICursor()
{
	SAFE_DELETE(mySprite);
}

void GUICursor::Render()
{
	mySprite->Render();
}

void GUICursor::SetPositionAgain(const CU::Vector2f& aPosition)
{
	mySprite->SetPosition(aPosition);
	myHasRealPos = true;
}

void GUICursor::SetPosition(const CU::Vector2f& aPosition)
{
	if (myHasRealPos)
	{
		return;
	}
	//const CU::Vector2f windowSize(WINDOW_SIZE);

	//CU::Vector2f position = aPosition;
	//if (aPosition.x > 1.f - 1.f / windowSize.x)
	//{
	//	position.x = 1.f - 1.f / windowSize.x;
	//}
	//else if (aPosition.x < 0.f)
	//{
	//	position.x = 0.f;
	//}

	//if (aPosition.y > 1.f - 1.f / windowSize.y)
	//{
	//	position.y = 1.f - 1.f / windowSize.y;
	//}
	//else if (aPosition.y < 0.f)
	//{
	//	position.y = 0.f;
	//}

	mySprite->SetPosition(/*position*/aPosition);
}

eMessageReturn GUICursor::Recieve(const Message& aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

const CU::Vector2f& GUICursor::GetPosition() const
{
	return mySprite->GetPosition();
}
