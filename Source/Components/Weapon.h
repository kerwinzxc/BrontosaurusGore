
#pragma once
#include "../Audio/AudioInterface.h"
struct SWeaponData;
class CGameObject;

namespace Physics
{
	class CPhysicsScene;
}

class CWeapon
{
public:
	CWeapon(SWeaponData* aWeaponData, Physics::CPhysicsScene* aPhysicsScene);
	~CWeapon();

	void TryToShoot(const CU::Vector3f& aDirection);
	void Shoot(const CU::Vector3f& aDirection);
	void Update(float aDeltaTime);
	void RotateXAxees(const float aRotationAmount);
	void SetModelVisibility(bool aVisibility);
	inline void SetUser(CGameObject* aUser);
	inline void SetWeaponObject(CGameObject* aWeaponObject);
	inline SWeaponData* GetData();
	bool CanShoot();

	void CosmeticShoot(const CU::Vector3f& aDirection); // Alex
private:
	enum class SoundEvent
	{
		Fire,
		Reload
	};
	void PlaySound(SoundEvent aSoundEvent, const CU::Vector3f& aDirection);

	CU::Vector3f RandomizedDirection(const CU::Vector3f& aDirection);
private:
	
	SWeaponData* myWeaponData;
	CGameObject* myUser;
	CGameObject* myWeaponObject;
	Physics::CPhysicsScene* myPhysicsScene;
	float myElapsedFireTimer;
	Audio::GameObjectID myAudioId;
	CU::Vector3f mySoundDirection;
};

inline void CWeapon::SetUser(CGameObject* aUser)
{
	myUser = aUser;
}

inline void CWeapon::SetWeaponObject(CGameObject* aWeaponObject)
{
	myWeaponObject = aWeaponObject;
}


inline SWeaponData* CWeapon::GetData()
{
	return myWeaponData;
}