#pragma once
class CGameObject;
struct SDamageOnCollisonCollidedObjectData
{
	CGameObject* collidedObject;
	float elapsedDamageCooldownTimer;
};