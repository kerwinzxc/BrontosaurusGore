#pragma once
struct WeaponData;
class CGameObject;
class Weapon
{
public:
	Weapon(WeaponData* aWeaponData);
	~Weapon();

	void TryToShoot(const CU::Vector3f& aDirection);
	void Shoot(const CU::Vector3f& aDirection);
	void Update(float aDeltaTime);
private:

	WeaponData* myWeaponData;
	CGameObject* myUser;
	float myElapsedFireTimer;

};

