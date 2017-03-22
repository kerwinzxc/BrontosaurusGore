#include "stdafx.h"
#include "LoadObject.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ServerLoadManager.h"
#include "Game/PlayState.h"
#include "GameObject.h"
#include "ComponentManager.h"
#include "DL_Debug.h"
#include "GameObjectManager.h"
#include "../TServer/GameServer.h"
#include "CommonUtilities.h"
#include "ComponentMessage.h"

int LoadObject(KLoader::SLoadedComponentData someData)
{
	LoadManager* loadManager = LoadManager::GetInstance();
	if (!loadManager) return NULL_COMPONENT;

	CGameObjectManager* gameObjectManager = loadManager->GetCurrentPLaystate().GetGameObjectManager();
	if (!gameObjectManager) return NULL_COMPONENT;

	CGameObject* const gameObject = gameObjectManager->CreateGameObject();

	CU::Matrix44f& currentMatrix = gameObject->GetLocalTransform();

	CU::CJsonValue PositionObject = someData.myData.at("position");

	const float unityScale = 1.0f;//100;

	const float positionX = PositionObject.at("x").GetFloat() * unityScale * -1;
	const float positionY = PositionObject.at("y").GetFloat() * unityScale;
	const float positionZ = PositionObject.at("z").GetFloat() * unityScale * -1;

	CU::CJsonValue RotationObject = someData.myData.at("rotation");

	const float rotationX = RotationObject.at("x").GetFloat() / 180 * PI;
	const float rotationY = RotationObject.at("y").GetFloat() / 180 * PI;
	const float rotationZ = RotationObject.at("z").GetFloat() / 180 * PI;

	CU::CJsonValue ScalingObject = someData.myData.at("scale");

	const float scaleX = ScalingObject.at("x").GetFloat();
	const float scaleY = ScalingObject.at("y").GetFloat();
	const float scaleZ = ScalingObject.at("z").GetFloat();

	currentMatrix.SetPosition({ positionX, positionY, positionZ });
	
	//Non euler code
	const CU::Matrix33f mRotationY = CU::Matrix33f::CreateRotateAroundY(rotationY);
	const CU::Matrix33f mRotationX = CU::Matrix33f::CreateRotateAroundX(-rotationX) * mRotationY;
	const CU::Matrix33f mRotationZ = CU::Matrix33f::CreateRotateAroundZ(rotationZ) * mRotationX;
	currentMatrix.SetRotation(mRotationZ);
	//what

	/*const CU::Vector3f rotation(rotationX, rotationY, rotationZ);
	currentMatrix.SetEulerRotation(rotation);

	const CU::Vector3f eulerRotation = currentMatrix.GetEulerRotation();

	currentMatrix.SetEulerRotation(eulerRotation);
*/
	currentMatrix.Scale({ scaleX, scaleY, scaleZ });

	gameObject->SetName(someData.myData.at("name").GetString().c_str());

	return gameObject->GetId();
}

void LinkObject(const int target, const int anOtherComponent)
{
	CComponent* const objectComponent = CComponentManager::GetInstance().GetComponent(target);
	CGameObject* const gameObject = (objectComponent->IsGameObject()) ? static_cast<CGameObject*>(objectComponent) : nullptr;

	if (gameObject == nullptr)
	{
		DL_ASSERT("ERROR: component is not a game object !!");
		return;
	}

	CComponent* const otherComponent = CComponentManager::GetInstance().GetComponent(anOtherComponent);

	const CU::Matrix44f transform = gameObject->GetToWorldTransform();

	const CU::Matrix44f& localTransform = gameObject->GetLocalTransform();
	const CGameObject*const  parent = gameObject->GetParent();

	gameObject->AddComponent(otherComponent);
}

int LoadServerObject(KLoader::SLoadedComponentData someData)
{
	CServerLoadManager* loadManager = CServerLoadManager::GetInstance();
	if (!loadManager) return NULL_COMPONENT;

	CGameObjectManager* gameObjectManager = &loadManager->GetCurrentGameServer().GetGameObjectManager();
	if (!gameObjectManager) return NULL_COMPONENT;

	CGameObject* const gameObject = gameObjectManager->CreateGameObject();

	CU::Matrix44f& currentMatrix = gameObject->GetLocalTransform();

	CU::CJsonValue PositionObject = someData.myData.at("position");

	const float unityScale = 1;

	const float positionX = PositionObject.at("x").GetFloat() * unityScale * -1;
	const float positionY = PositionObject.at("y").GetFloat() * unityScale;
	const float positionZ = PositionObject.at("z").GetFloat() * unityScale * -1;

	currentMatrix.SetPosition({ positionX, positionY, positionZ });

	CU::CJsonValue RotationObject = someData.myData.at("rotation");

	const float rotationX = RotationObject.at("x").GetFloat() / 180 * PI;
	const float rotationY = RotationObject.at("y").GetFloat() / 180 * PI;
	const float rotationZ = RotationObject.at("z").GetFloat() / 180 * PI;

	currentMatrix.Rotate(rotationX, rotationY, rotationZ);

	CU::CJsonValue ScalingObject = someData.myData.at("scale");

	const float scaleX = ScalingObject.at("x").GetFloat();
	const float scaleY = ScalingObject.at("y").GetFloat();
	const float scaleZ = ScalingObject.at("z").GetFloat();

	currentMatrix.Scale({ scaleX, scaleY, scaleZ });

	gameObject->SetName(someData.myData.at("name").GetString().c_str());

	return gameObject->GetId();
}

