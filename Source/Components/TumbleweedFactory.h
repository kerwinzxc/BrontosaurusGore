#pragma once

class CTumbleweedController;
class CGameObjectManager;
class CColliderComponentManager;

class CTumbleweedFactory
{
public:
	~CTumbleweedFactory();

	static void CreateInstance();
	static CTumbleweedFactory* GetInstance();
	static void DestroyInstance();
	int CreateNewTumbleweed(const CU::Vector3f& aPosition);
	void Update(const float aDeltaTime);
	void Init(CGameObjectManager* aGameObjectManager, CColliderComponentManager* aColliderComponentManager);
private:
	CTumbleweedFactory();
	
private:
	static CTumbleweedFactory* ourInstance;
	CU::GrowingArray<CTumbleweedController*> myTumbleWeedControllers;
	CColliderComponentManager* myColliderComponentManagerPointer;
	CGameObjectManager* myGameObjectManager;
};

