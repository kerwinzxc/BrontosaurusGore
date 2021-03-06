#pragma once
#include "../ThreadedPostmaster/Subscriber.h"

struct SExplosionData;
struct SExplosionBufferData;
class CGameObjectManager;
class CModelComponentManager;
class CExplosionComponentManager;
class CColliderComponentManager;
class CExplosionFactory : public Postmaster::ISubscriber
{
public:
	CExplosionFactory(CExplosionComponentManager* aExplosionComponentManager);
	~CExplosionFactory();
	void Init(CGameObjectManager* aGameObjectManager, CModelComponentManager* aModelComponentManagerPointer, CColliderComponentManager* aColliderComponentManager);
	eMessageReturn DoEvent(const CCreateExplosionMessage& aCreateExplosionMessage) override;
	eMessageReturn DoEvent(const CDeactivateExplosionMessage& aDeactivateExplosionMessage) override;
private:
	void CreateExplosion();
private:
	CU::GrowingArray<SExplosionBufferData*> myActiveExplosions;
	CU::GrowingArray<SExplosionBufferData*> myPassiveExplosions;
	CGameObjectManager* myGameObjectManagerPointer;
	CModelComponentManager* myModelComponentManagerPointer;
	CExplosionComponentManager* myExplosionComponentsManager;
	CColliderComponentManager* myColliderComponentManagerPointer;
};

