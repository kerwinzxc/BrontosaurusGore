#include "stdafx.h"
#include "Physics.h"


#include <foundation\PxTransform.h>
#include <PxPhysics.h>
#include <PxScene.h>
#include <PxSceneDesc.h>
#include <PxPhysicsAPI.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>


#include "Foundation.h"
#include "PhysicsScene.h"
#include "Shape.h"
#include "PhysicsActorDynamic.h"
#include "PhysicsActorStatic.h"
#include "SimulationEventCallback.h"
#include "CollisionLayers.h"

namespace Physics
{
	using namespace physx;

	namespace
	{
		physx::PxFilterFlags CollisionFilterShader(
			physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
			physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
			physx::PxPairFlags& pairFlags, const void* /*constantBlock*/, PxU32 /*constantBlockSize*/)
		{
			// let triggers through
			if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
			{
				pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
				return physx::PxFilterFlag::eDEFAULT;
			}
			// generate contacts for all that were not filtered above
			pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

			// trigger the contact callback for pairs (A,B) where 
			// the filtermask of A contains the ID of B and vice versa.
			if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
			{
				pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_LOST | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
				return physx::PxFilterFlag::eDEFAULT;//Remove this if collision filter is messing up
			}

			return physx::PxFilterFlag::eSUPPRESS;//Change back to Default if collision filter is messing up
		}
	}


	CPhysics::CPhysics(PxPhysics* aPxPhysics)
	{
		myPxPhysics = aPxPhysics;
		myDefaultMaterial = myPxPhysics->createMaterial(defMatStaticFriction, defMatDynamicFriction, defMatRestitution);
		myEventCallback = new CSimulationEventCallback();
		myDispatcher = PxDefaultCpuDispatcherCreate(0);

	}

	CPhysics::~CPhysics()
	{
		SAFE_RELEASE(myPxPhysics);
		SAFE_RELEASE(myDispatcher);

		SAFE_DELETE(myEventCallback);
		//SAFE_RELEASE(myDefaultMaterial);
	}

	CPhysicsScene* CPhysics::CreateScene(const CU::Vector3f aGravity)
	{
		PxScene* pxScene = nullptr;
		PxSceneDesc sceneDesc(myPxPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(aGravity.x, aGravity.y, aGravity.z);

	

		sceneDesc.simulationEventCallback = myEventCallback;
		sceneDesc.cpuDispatcher = myDispatcher;

		//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		sceneDesc.filterShader = CollisionFilterShader;


		pxScene = myPxPhysics->createScene(sceneDesc);

#ifndef _RETAIL_BUILD
		PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::Enum::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::Enum::eTRANSMIT_SCENEQUERIES, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::Enum::eTRANSMIT_CONSTRAINTS, true);
		}
#endif
		return new CPhysicsScene(pxScene);

	}

	CPhysicsActor* CPhysics::CreateStaticActor(CShape* aShape, const bool aIsTrigger)
	{
		PxTransform transform(0, 0, 0);
		PxRigidStatic* body = myPxPhysics->createRigidStatic(transform);
		if(aShape != nullptr)
			body->attachShape(*aShape->myShape);
		CPhysicsActorStatic* actor = new CPhysicsActorStatic(body, aShape);
		actor->SetIsTrigger(aIsTrigger);

		return actor;
	}

	CPhysicsActor* CPhysics::CreateDynamicActor(CShape* aShape, const bool aIsTrigger, const float aMass, const bool aIsKinematic, const bool aUseGravity)
	{
		PxTransform transform(0, 0, 0);
		PxRigidDynamic* body = myPxPhysics->createRigidDynamic(transform);
		if (aShape != nullptr)
			body->attachShape(*aShape->myShape);
		CPhysicsActorDynamic* actor = new CPhysicsActorDynamic(body, aShape);
		actor->SetIsTrigger(aIsTrigger);
		actor->SetIsKinematic(aIsKinematic);
		PxRigidBodyExt::updateMassAndInertia(*body, aMass);
		return actor;
	}

	CShape* CPhysics::CreateBoxShape(const CU::Vector3f& aHalfExtent, const SMaterialData & aMaterialData)
	{
		PxVec3 size = { aHalfExtent.x, aHalfExtent.y, aHalfExtent.z };
		PxBoxGeometry* geometry = new PxBoxGeometry(size);
		return CreateShape(*geometry, aMaterialData);
	}
	CShape* CPhysics::CreateSphereShape(const float aRadius, const SMaterialData & aMaterialData)
	{
		PxSphereGeometry* geometry = new PxSphereGeometry(); 
		geometry->radius = aRadius;
		return CreateShape(*geometry, aMaterialData);
	}

	Physics::CShape* CPhysics::CreateCapsuleShape(const float aRadius, const float aHalfHeight, const SMaterialData& aMaterialData)
	{
		PxCapsuleGeometry* geometry = new PxCapsuleGeometry();
		geometry->radius = aRadius;
		geometry->halfHeight = aHalfHeight;
		return CreateShape(*geometry, aMaterialData);
	}

	CShape* CPhysics::CreateMeshShape(const char * aPath, const SMaterialData & aMaterialData)
	{
		DL_ASSERT("Mesh colliders not implemented yet.");
		PxTriangleMeshDesc desc;
		return nullptr;
	}

	CShape* CPhysics::CreateShape(PxGeometry& aGeometry, const SMaterialData & aMaterialData)
	{
		PxMaterial* material = CreateMaterial(aMaterialData);
		PxShape* shape = myPxPhysics->createShape(aGeometry, *material);
		//release material?
		return new CShape(shape);
	}


	PxMaterial* CPhysics::CreateMaterial(const SMaterialData & aMaterialData)
	{
		if (aMaterialData.aDynamicFriction == defMatDynamicFriction &&
			aMaterialData.aStaticFriction == defMatStaticFriction &&
			aMaterialData.aRestitution == defMatRestitution)
		{
			return myDefaultMaterial;
		}
		return myPxPhysics->createMaterial(aMaterialData.aStaticFriction, aMaterialData.aDynamicFriction, aMaterialData.aRestitution);
	}



}