#include "stdafx.h"
#include "TumbleweedFactory.h"
#include "TumbleweedController.h"
#include "GameObjectManager.h"
#include "ColliderComponentManager.h"
#include "ModelComponentManager.h"
#include "SphereColliderComponent.h"
#include "RigidBodyComponent.h"

CTumbleweedFactory* CTumbleweedFactory::ourInstance = nullptr;

CTumbleweedFactory::CTumbleweedFactory()
{
	myTumbleWeedControllers.Init(20);
}


CTumbleweedFactory::~CTumbleweedFactory()
{
}

void CTumbleweedFactory::CreateInstance()
{
	ourInstance = new CTumbleweedFactory();
}
CTumbleweedFactory* CTumbleweedFactory::GetInstance()
{
	return ourInstance;
}
void CTumbleweedFactory::DestroyInstance()
{
	SAFE_DELETE(ourInstance);
}

int CTumbleweedFactory::CreateNewTumbleweed(const CU::Vector3f& aPosition)
{
	CGameObject* tumbleweedObject = myGameObjectManager->CreateGameObject();
	CModelComponent* modelComponent = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_thumbleweed_01.fbx");
	SSphereColliderData sphereData;
	sphereData.myRadius = 1.25f;
	sphereData.material.aDynamicFriction = 50.f;
	sphereData.material.aStaticFriction = 50.f;
	CColliderComponent* sphereColliderComponent = myColliderComponentManagerPointer->CreateComponent(&sphereData, tumbleweedObject->GetId());
	SRigidBodyData rigidBodyData;
	rigidBodyData.mass = 2.0f;
	CColliderComponent* rigidBodyComponent = myColliderComponentManagerPointer->CreateComponent(&rigidBodyData, tumbleweedObject->GetId());
	CTumbleweedController* tumbleWeedController = new CTumbleweedController();
	tumbleWeedController->Init(myColliderComponentManagerPointer->GetScene());
	CComponentManager::GetInstance().RegisterComponent(tumbleWeedController);
	myTumbleWeedControllers.Add(tumbleWeedController);
	tumbleweedObject->AddComponent(modelComponent);
	tumbleweedObject->AddComponent(sphereColliderComponent);
	tumbleweedObject->AddComponent(rigidBodyComponent);
	tumbleweedObject->AddComponent(tumbleWeedController);
	tumbleweedObject->GetLocalTransform().SetPosition(aPosition);
	tumbleweedObject->NotifyComponents(eComponentMessageType::eObjectDone, SComponentMessageData());
	tumbleWeedController->AddForce();
	return tumbleweedObject->GetId();
}

int CTumbleweedFactory::CreateNewTumbleweed(const CU::Vector3f& aPosition, const CU::Vector3f& aDirection)
{
	CGameObject* tumbleweedObject = myGameObjectManager->CreateGameObject();
	CModelComponent* modelComponent = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_thumbleweed_01.fbx");

	SSphereColliderData sphereData;
	sphereData.myRadius = 1.25f;
	sphereData.material.aDynamicFriction = 50.f;
	sphereData.material.aStaticFriction = 50.f;
	CColliderComponent* sphereColliderComponent = myColliderComponentManagerPointer->CreateComponent(&sphereData, tumbleweedObject->GetId());
	SRigidBodyData rigidBodyData;
	rigidBodyData.mass = 2.0f;
	CColliderComponent* rigidBodyComponent = myColliderComponentManagerPointer->CreateComponent(&rigidBodyData, tumbleweedObject->GetId());
	CTumbleweedController* tumbleWeedController = new CTumbleweedController();
	tumbleWeedController->Init(myColliderComponentManagerPointer->GetScene());
	tumbleWeedController->SetDirection(aDirection);
	CComponentManager::GetInstance().RegisterComponent(tumbleWeedController);
	myTumbleWeedControllers.Add(tumbleWeedController);
	tumbleweedObject->AddComponent(modelComponent);
	tumbleweedObject->AddComponent(sphereColliderComponent);
	tumbleweedObject->AddComponent(rigidBodyComponent);
	tumbleweedObject->AddComponent(tumbleWeedController);
	tumbleweedObject->GetLocalTransform().SetPosition(aPosition);
	tumbleweedObject->NotifyComponents(eComponentMessageType::eObjectDone, SComponentMessageData());
	tumbleWeedController->AddForce();
	return tumbleweedObject->GetId();
}
void CTumbleweedFactory::Update(const float aDeltaTime)
{
	for(unsigned int i = 0; i < myTumbleWeedControllers.Size(); i++)
	{
		myTumbleWeedControllers[i]->Update(aDeltaTime);
	}
}

void CTumbleweedFactory::Init(CGameObjectManager* aGameObjectManager, CColliderComponentManager* aColliderComponentManager)
{
	myGameObjectManager = aGameObjectManager;
	myColliderComponentManagerPointer = aColliderComponentManager;
}