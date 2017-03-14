#pragma once
#include "../ThreadedPostmaster/Message.h"
class CGameObject;
class CAddToCheckPointResetList : public Postmaster::Message::IMessage
{
public:
	CAddToCheckPointResetList(CGameObject* aGameObject);
	~CAddToCheckPointResetList();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	inline CGameObject* GetObjectToReset() const;
private:
	CGameObject* myObjectToReset;
};

inline CGameObject* CAddToCheckPointResetList::GetObjectToReset() const
{
	return myObjectToReset;
}