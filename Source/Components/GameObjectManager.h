#pragma once
#include "../CommonUtilities/matrix44.h"
#include "Component.h"
#include "..\CommonUtilities\stack.h"


class CGameObject;

class CGameObjectManager
{
public:
	CGameObjectManager();
	~CGameObjectManager();

	CGameObject* CreateGameObject();
	CGameObject* CreateGameObject(const CU::Matrix44f& aMatrix);

	CU::Matrix44f& GetTransform(CGameObject& aGameObject);

	void DestroyObject(CGameObject* aGameObject);
	void AddObjectForDestruction(CGameObject* aGameObject);
	
	void DestroyObjectsWaitingForDestruction();

	void ClearAll();
	void DumpAllAndReInit(); // this is the unsafe way to destroy everything with the assumption that the game objects don't own anything, mvh carl

	void SendObjectsDoneMessage();

private:
	CU::GrowingArray<CU::Matrix44f> myMatrices;
	CU::Stack<ComponentId> myFreeMatrices;

	void InitGrowingArrays();
	ComponentId CreateMatrix(const CU::Matrix44f& aMatrix44f);
	
	CU::GrowingArray<CGameObject*> myObjectsAboutToBeDestroyed;
	CU::GrowingArray<CGameObject*> myObjectsCreated;
};

inline CGameObject* CGameObjectManager::CreateGameObject()
{
	return CreateGameObject(CU::Matrix44f::Identity);
}
