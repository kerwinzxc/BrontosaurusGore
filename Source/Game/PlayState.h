#pragma once
#include "../StateStack/State.h"
#include "../PostMaster/Subscriber.h"
#include <atomic>
#include "../Components/EnemyComponentManager.h"
#include "../ThreadedPostmaster/Subscriber.h"
#include "../GUI/HUD.h"

namespace CU
{
	class Time;
	class Camera;
}

namespace Physics
{
	class CPhysicsScene;
	class CPhysics;
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
class CColliderComponentManager;
class CCheckPointSystem;
class CScriptComponentManager;
class CExplosionFactory;
class CExplosionComponentManager;
class CTextInstance;
class CDamageOnCollisionComponentManager;
class CGameObject;

class CPlayState : public State , public Postmaster::ISubscriber
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


	void SpawnOtherPlayer(unsigned aPlayerID);

	CGameObjectManager* GetGameObjectManager();
	inline CWeaponSystemManager* GetCWeaponSystemManager();
	inline CAmmoComponentManager* GetAmmoManager();
	inline CMovementComponentManager* GetMovementComponentManager();
	inline CColliderComponentManager* GetColliderComponentManager();
	inline CEnemyComponentManager* GetEnemyComponentManager();
	inline CScriptComponentManager* GetScriptComponentManager();
	inline CDamageOnCollisionComponentManager* GetDamageOnCollisionComponentManager();

	inline bool IsLoaded() const;

	eMessageReturn DoEvent(const CLoadLevelMessage& aLoadLevelMessage) override;
	CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;

private:

	void CreatePlayer(CU::Camera& aCamera);
	void GivePlayerWeapons(CGameObject* aPlayerObject);
private:
	Physics::CPhysicsScene* myPhysicsScene;
	Physics::CPhysics* myPhysics;


	CColliderComponentManager* myColliderComponentManager;

	CHUD myHUD;

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
	CScriptComponentManager* myScriptComponentManager;
	CExplosionFactory* myExplosionFactory;
	CExplosionComponentManager* myExplosionComponentManager;
	CDamageOnCollisionComponentManager* myDamageOnCollisionComponentManager;

	CMovementComponent* myMovementComponent;
	CCheckPointSystem* myCheckPointSystem;
	class CCameraComponent* myCameraComponent;
	friend struct SEncapsulationBreaker;

	int myLevelIndex;
	std::atomic_bool myIsLoaded;

	//Super temp ta bort när guit börjar ta form
	CTextInstance* myPlayerHealthText;
	CTextInstance* myPlayerArmorText;

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

inline CMovementComponentManager* CPlayState::GetMovementComponentManager()
{
	return myMovementComponentManager;
}

inline CColliderComponentManager* CPlayState::GetColliderComponentManager()
{
	return myColliderComponentManager;
}

inline CEnemyComponentManager* CPlayState::GetEnemyComponentManager()
{
	return myEnemyComponentManager;
}

inline CScriptComponentManager* CPlayState::GetScriptComponentManager()
{
	return myScriptComponentManager;
}

inline CDamageOnCollisionComponentManager* CPlayState::GetDamageOnCollisionComponentManager()
{
	return myDamageOnCollisionComponentManager;
}