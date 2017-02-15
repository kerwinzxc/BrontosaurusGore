#pragma once
#include "RigidBody.h"
#include "../Components/Component.h"


class CPhysicsManager
{
public:

	static CPhysicsManager &GetInstance();
	static bool CreateInstance();
	static void DestroyInstance();

	void Update();



private:
	CPhysicsManager();
	~CPhysicsManager();

	static CPhysicsManager* ourInstance;
	static int ourRefCount;

};