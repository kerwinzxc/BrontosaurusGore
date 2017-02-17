#include "stdafx.h"
#include "LoadObject.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "Game/PlayState.h"
#include "GameObject.h"
#include "ComponentManager.h"
#include "DL_Debug.h"
#include "GameObjectManager.h"
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

	const float unityScale = 100;

	const float positionX = PositionObject.at("x").GetFloat() * unityScale * -1;
	const float positionY = PositionObject.at("y").GetFloat() * unityScale;
	const float positionZ = PositionObject.at("z").GetFloat() * unityScale * -1;

	currentMatrix.SetPosition({positionX, positionY, positionZ});

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

	gameObject->AddComponent(otherComponent);
}
