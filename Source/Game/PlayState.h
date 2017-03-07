#pragma once
#include "../StateStack/State.h"
#include "../PostMaster/Subscriber.h"
#include <atomic>
#include "../Components/EnemyComponentManager.h"
#include "../ThreadedPostmaster/Subscriber.h"

namespace CU
{
	class Time;
	class Camera;
}

class CGameObjectManager;
class CScene;
class CModelComponentManager;
class CCollisionComponentManager;
class CAmmoComponentManager;
class CWeaponSystemManager;
class CWeaponFactory;
class CProjectileComponentManager;
class CProjectileFactory;
class CAmmoComponentManager;
class CWeaponSystemManager;
class CMovementComponent;
class CWeaponFactory;
class CProjectileComponentManager;
class CProjectileFactory;
class CInputComponentManager;
class CNetworkComponentManager;
class CMovementComponentManager;

class CPlayState : public State/* , public Postmaster::ISubscriber*/
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
	inline CWeaponSystemManager* GetCWeaponSystemManager();
	inline CAmmoComponentManager* GetAmmoManager();
	inline CMovementComponentManager* GetMovementComponentManager();
	inline bool IsLoaded() const;
private:
	void TempHardCodePlayerRemoveTHisLaterWhenItIsntNecessaryToHaveAnymore(CU::Camera& aCamera);
private:
	CGameObjectManager* myGameObjectManager;
	CScene* myScene;

	CModelComponentManager* myModelComponentManager;

	CAmmoComponentManager* myAmmoComponentManager;
	CWeaponSystemManager* myWeaponSystemManager;
	CWeaponFactory* myWeaponFactory;
	CProjectileComponentManager* myProjectileComponentManager;
	CProjectileFactory* myProjectileFactory;
	CEnemyComponentManager* myEnemyComponentManager;
	CInputComponentManager* myInputComponentManager;
	CMovementComponentManager* myMovementComponentManager;

	CMovementComponent* myMovementComponent;
	class CCameraComponent* myCameraComponent;
	friend struct SEncapsulationBreaker;

	int myLevelIndex;
	std::atomic_bool myIsLoaded;
	
};

inline bool CPlayState::IsLoaded() const
{
	return myIsLoaded;
}

inline CWeaponSystemManager* CPlayState::GetCWeaponSystemManager()
{
	return myWeaponSystemManager;
}

inline CAmmoComponentManager* CPlayState::GetAmmoManager()
{
	return myAmmoComponentManager;
}

inline CMovementComponentManager * CPlayState::GetMovementComponentManager()
{
	return myMovementComponentManager;
}
