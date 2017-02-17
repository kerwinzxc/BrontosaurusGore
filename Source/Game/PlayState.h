#pragma once
#include "../StateStack/State.h"
#include "../PostMaster/Subscriber.h"
#include <atomic>

namespace CU
{
	class Time;
}

class CCollisionComponentManager;
class CGameObjectManager;
class CScene;
class AmmoComponentManager;
class WeaponSystemManager;
class WeaponFactory;
class ProjectileComponentManager;
class ProjectileFactory;

class CMovementComponent;

class CPlayState : public State , public Subscriber
{
public:
	CPlayState(StateStack& aStateStack, const int aLevelIndex);
	~CPlayState();

	void Load();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;
	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	void Pause();
	void CreateManagersAndFactories();

	eMessageReturn Recieve(const Message& aMessage);
	CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;

	CGameObjectManager* GetGameObjectManager();

	inline bool IsLoaded() const;

private:
	CGameObjectManager* myGameObjectManager;
	CScene* myScene;
	AmmoComponentManager* myAmmoComponentManager;
	WeaponSystemManager* myWeaponSystemManager;
	WeaponFactory* myWeaponFactory;
	ProjectileComponentManager* myProjectileComponentManager;
	ProjectileFactory* myProjectileFactory;

	CMovementComponent* myMovementComponent;

	std::atomic_bool  myIsLoaded;
	int myLevelIndex;
};

inline bool CPlayState::IsLoaded() const
{
	return myIsLoaded;
}

