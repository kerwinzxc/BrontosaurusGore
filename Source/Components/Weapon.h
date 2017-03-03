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
	inline void SetUser(CGameObject* aUser);
	inline SWeaponData* GetData();
private:

	SWeaponData* myWeaponData;
	CGameObject* myUser;
	float myElapsedFireTimer;

};

inline void CWeapon::SetUser(CGameObject* aUser)
{
	myUser = aUser;
}

inline SWeaponData* CWeapon::GetData()
{
	return myWeaponData;
}