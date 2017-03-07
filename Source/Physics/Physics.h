#pragma once

class CShape;

namespace physx
{
	class PxPhysics;
	class PxDefaultCpuDispatcher;
	class PxMaterial;
}

struct SMaterialData
{
	float aStaticFriction;
	float aDynamicFriction;
	float aRestitution;
};

namespace Physics
{
	class CPhysicsScene;
	class CPhysics
	{

		class CFoundation;

	public:
		CPhysics(physx::PxPhysics* aPxPhysics);
		~CPhysics();

		CPhysicsScene* CreateNewScene();

		CShape* CreateBoxShape(const CU::Vector3f& aSize, const SMaterialData& aMaterialData);
		CShape* CreateSphereShape(const float aRadius, const SMaterialData& aMaterialData);
		CShape* CreateMeshShape(const char* aPath, const SMaterialData& aMaterialData);

	private:
		CShape* CreateShape(physx::PxGeometry& aGeometry, const SMaterialData & aMaterialData);
		physx::PxMaterial* CreateMaterial(const SMaterialData & aMaterialData);

	private:
		physx::PxPhysics* myPxPhysics;
		physx::PxDefaultCpuDispatcher* myDispatcher;
		physx::PxMaterial* myDefaultMaterial;
		CU::GrowingArray<CPhysicsScene*> myScenes;
	};
}
