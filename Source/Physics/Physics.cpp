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

namespace Physics
{
	using namespace physx;




	CPhysics::CPhysics(PxPhysics* aPxPhysics)
	{
		myPxPhysics = aPxPhysics;
		myDefaultMaterial = myPxPhysics->createMaterial(defMatStaticFriction, defMatDynamicFriction, defMatRestitution);
	}

	CPhysics::~CPhysics()
	{
		SAFE_RELEASE(myPxPhysics);
		//SAFE_RELEASE(myDefaultMaterial);
	}

	CPhysicsScene* CPhysics::CreateScene(const CU::Vector3f aGravity)
	{
		PxScene* pxScene = nullptr;
		PxSceneDesc sceneDesc(myPxPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(aGravity.x, aGravity.y, aGravity.z);

		myDispatcher = PxDefaultCpuDispatcherCreate(0);
		sceneDesc.cpuDispatcher = myDispatcher;
		//maybe GPU Dispatcher
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
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
	CShape* CPhysics::CreateMeshShape(const char * aPath, const SMaterialData & aMaterialData)
	{
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