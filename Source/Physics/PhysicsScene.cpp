#include "stdafx.h"
#include "PhysicsScene.h"
#include <PxScene.h>
#include <PxRigidActor.h>
#include "PhysicsActor.h"


namespace Physics
{
	CPhysicsScene::CPhysicsScene(physx::PxScene* aPxScene)
	{
		myPxScene = aPxScene;
		mySimulationTimer = 0.f;
	}

	CPhysicsScene::~CPhysicsScene()
	{
		if (myPxScene)
			myPxScene->release();
		myPxScene = nullptr;
	}

	bool CPhysicsScene::Simulate(const CU::Time aDeltaTime)
	{
		// aDeltaT shuld be 1/60
		// Fix with timers bby
		//TODO: if main FPS is below 60? 

		mySimulationTimer += aDeltaTime.GetSeconds();
		if (mySimulationTimer >= ourSimulationFrequensy)
		{
			mySimulationTimer -= ourSimulationFrequensy;
			myPxScene->simulate(ourSimulationFrequensy);
			myPxScene->fetchResults();
			return true;
		}
		return false;
	}

	Physics::SRaycastHitData CPhysicsScene::Raycast(const CU::Vector3f& aOrigin, const CU::Vector3f& aDirection, float aRayLength)
	{
		SRaycastHitData outData;
		physx::PxRaycastBuffer hit;
		physx::PxVec3 origin = { aOrigin.x, aOrigin.y, aOrigin.z };
		physx::PxVec3 dir = { aDirection.x, aDirection.y, aDirection.z };
		physx::PxReal dist = aRayLength;

		if (myPxScene->raycast(origin, dir, dist, hit))
		{
			outData.hit = hit.hasBlock;
			if(hit.hasBlock)
			{
				outData.distance = hit.block.distance;
				outData.position = { hit.block.position.x, hit.block.position.y, hit.block.position.z };
				outData.normal = { hit.block.normal.x, hit.block.normal.y, hit.block.normal.z };
				outData.faceIndex = hit.block.faceIndex;
				outData.actor = static_cast<CPhysicsCallbackActor*>(hit.block.actor->userData);
				return outData;
			}
		}
		return outData;
	}

	void CPhysicsScene::AddActor(CPhysicsActor* aActor)
	{
		myPxScene->addActor(*aActor->GetPxActor());
	}

	void CPhysicsScene::RemoveActor(CPhysicsActor* aActor)
	{
		myPxScene->removeActor(*aActor->GetPxActor());
	}

	
}
