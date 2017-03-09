
#pragma once
struct SWeaponData;
class CGameObject;
class CWeapon
{
public:
	CWeapon(SWeaponData* aWeaponData);
	~CWeapon();

	void TryToShoot(const CU::Vector3f& aDirection);
	void Shoot(const CU::Vector3f& aDirection);
	void Update(float aDeltaTime);
	void RotateXAxees(const float aRotationAmount);
	void SetModelVisibility(bool aVisibility);
	inline void SetUser(CGameObject* aUser);
	inline void SetWeaponObject(CGameObject* aWeaponObject);
	inline SWeaponData* GetData();
private:

	SWeaponData* myWeaponData;
	CGameObject* myUser;
	CGameObject* myWeaponObject;
	float myElapsedFireTimer;

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