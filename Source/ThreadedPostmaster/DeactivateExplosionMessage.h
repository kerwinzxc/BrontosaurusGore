#pragma once
#include "../ThreadedPostmaster/Message.h"

class CGameObject;
class CDeactivateExplosionMessage : public Postmaster::Message::IMessage
{
public:
	CDeactivateExplosionMessage(CGameObject* aGameObject);
	~CDeactivateExplosionMessage();

	IMessage* Copy() override;
	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	inline CGameObject* GetGameObject() const;
private:
	CGameObject* myGameObject;
};

inline CGameObject* CDeactivateExplosionMessage::GetGameObject() const
{
	return myGameObject;
}