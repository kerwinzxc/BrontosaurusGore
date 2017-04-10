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
#include "Collection.h"
#include <iostream>

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

			if (filterData0.word2 == filterData1.word2)
			{
				int i = 0;
			}
			// trigger the contact callback for pairs (A,B) where 
			// the filtermask of A contains the ID of B and vice versa.
			if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1) /*&& filterData0.word2 != filterData1.word2*/)
			{
				//std::cout << "First object: " << filterData0.word2 << "\t\tSecond object: " << filterData1.word2 << std::endl;
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
		actor->SetUseGravity(aUseGravity);
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

	CShape* CPhysics::CreateMeshShape(const char * aPath, const SMaterialData & aMaterialData, const CU::Vector3f& aScale)
	{
		PxDefaultFileInputData inputData(aPath);
		PxSerializationRegistry* registry = PxSerialization::createSerializationRegistry(PxGetPhysics());
		PxCollection* collection = PxSerialization::createCollectionFromXml(inputData,
			Physics::CFoundation::GetInstance()->GetCooking(), *registry);

		CShape* shape = nullptr;

		const PxU32 size = collection->getNbObjects();
		for (PxU32 i = 0; i < size && shape == nullptr; ++i)
		{
			PxBase* obj = &collection->getObject(i);
			//if (!obj->is<PxActor>())
				//continue;

			const PxConcreteType::Enum type = static_cast<PxConcreteType::Enum>(obj->getConcreteType());
			const int x = 0;
			switch (type)
			{
			case PxConcreteType::eUNDEFINED: break;
			case PxConcreteType::eHEIGHTFIELD: break;
			case PxConcreteType::eCONVEX_MESH:
				{

					PxConvexMesh* mesh = static_cast<PxConvexMesh*>(obj);

					shape = CreateShape(*mesh, aMaterialData, aScale);
				}
				break;
			case PxConcreteType::eTRIANGLE_MESH_BVH33: 
			{
				PxTriangleMesh* mesh = static_cast<PxTriangleMesh*>(obj);
				
				shape = CreateShape(*mesh, aMaterialData);
			}
			case PxConcreteType::eTRIANGLE_MESH_BVH34: 
			{
				PxTriangleMesh* mesh = static_cast<PxTriangleMesh*>(obj);

				shape = CreateShape(*mesh, aMaterialData);
			}
				break;
			case PxConcreteType::eCLOTH_FABRIC: break;
			case PxConcreteType::eRIGID_DYNAMIC: break;
			case PxConcreteType::eRIGID_STATIC: break;
			case PxConcreteType::eSHAPE: break;
			case PxConcreteType::eMATERIAL: break;
			case PxConcreteType::eCONSTRAINT: break;
			case PxConcreteType::eCLOTH: break;
			case PxConcreteType::ePARTICLE_SYSTEM: break;
			case PxConcreteType::ePARTICLE_FLUID: break;
			case PxConcreteType::eAGGREGATE: break;
			case PxConcreteType::eARTICULATION: break;
			case PxConcreteType::eARTICULATION_LINK: break;
			case PxConcreteType::eARTICULATION_JOINT: break;
			case PxConcreteType::ePRUNING_STRUCTURE: break;
			case PxConcreteType::ePHYSX_CORE_COUNT: break;
			case PxConcreteType::eFIRST_PHYSX_EXTENSION: break;
			case PxConcreteType::eFIRST_VEHICLE_EXTENSION: break;
			case PxConcreteType::eFIRST_USER_EXTENSION: break;
			default: break;
			}
		}
		return shape;
	}

	CShape* CPhysics::CreateShape(PxGeometry& aGeometry, const SMaterialData & aMaterialData)
	{
		PxMaterial* material = CreateMaterial(aMaterialData);
		PxShape* shape = myPxPhysics->createShape(aGeometry, *material);
		//release material?
		return new CShape(shape);
	}

	CShape* CPhysics::CreateShape(physx::PxTriangleMesh& aMesh, const SMaterialData & aMaterialData)
	{
		PxMaterial* material = CreateMaterial(aMaterialData);

		PxTriangleMeshGeometry* meshGeometry = new PxTriangleMeshGeometry(&aMesh);
		PxShape* shape = myPxPhysics->createShape(*meshGeometry,*material);
		return new CShape(shape);
	}

	CShape* CPhysics::CreateShape(physx::PxConvexMesh& aMesh, const SMaterialData& aMaterialData, const CU::Vector3f& aScale)
	{
		PxMaterial* material = CreateMaterial(aMaterialData);

		PxConvexMeshGeometry* meshGeometry = new PxConvexMeshGeometry(&aMesh);
		PxMeshScale& scale = meshGeometry->scale;
		scale.scale.x = aScale.x;
		scale.scale.y = aScale.y;
		scale.scale.z = aScale.z;
		PxShape* shape = myPxPhysics->createShape(*meshGeometry, *material);
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
