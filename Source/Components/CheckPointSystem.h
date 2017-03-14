#pragma once
#include "../ThreadedPostmaster/Subscriber.h"
class CGameObject;
class CCheckPointSystem : public Postmaster::ISubscriber
{
public:
	CCheckPointSystem();
	~CCheckPointSystem();

	eMessageReturn DoEvent(const CAddToCheckPointResetList& aAddToCheckPointResetList) override;
	eMessageReturn DoEvent(const CSetAsNewCheckPointMessage& aSetAsNewCheckPointMessage) override;
private:
	CU::GrowingArray<CGameObject*> myObjectsToReset;
	CU::Vector3f myRespawnPlayerPosition;
};

