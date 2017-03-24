#pragma once
namespace physx
{
	class PxPhysics;
	class PxDefaultCpuDispatcher;
	class PxMaterial;
	class PxGeometry;
	class PxControllerManager;
	class PxTriangleMesh;
	class PxConvexMesh;
}

namespace Physics
{
	class CPhysicsScene;
	class CPhysicsActor;
	class CPhysicsCharacterController;
	class CShape;
	class CCollection;
	class CSimulationEventCallback;
	struct SCharacterControllerInitParams;

	const float defMatStaticFriction = 0.5f;
	const float defMatDynamicFriction = 0.5f;
	const float defMatRestitution = 0.5f;

	struct SMaterialData
	{
		float aStaticFriction = defMatStaticFriction;
		float aDynamicFriction = defMatDynamicFriction;
		float aRestitution = defMatRestitution;
	};


	class CPhysics
	{
		class CFoundation;

	public:
		CPhysics(physx::PxPhysics* aPxPhysics);
		~CPhysics();

		CPhysicsScene* CreateScene(const CU::Vector3f aGravity = {0.0f, -9.82f, 0.0f});
		CPhysicsActor* CreateStaticActor(CShape* aShape, const bool aIsTrigger);
		CPhysicsActor* CreateDynamicActor(CShape* aShape, const bool aIsTrigger, const float aMass, const bool aIsKinematic , const bool aUseGravity);

		CShape* CreateBoxShape(const CU::Vector3f& aHalfExtent, const SMaterialData& aMaterialData = SMaterialData());
		CShape* CreateSphereShape(const float aRadius, const SMaterialData& aMaterialData);
		CShape* CreateCapsuleShape(const float aRadius, const float aHalfHeight, const SMaterialData& aMaterialData);
		CShape* CreateMeshShape(const char* aPath, const SMaterialData& aMaterialData);

		physx::PxMaterial* CreateMaterial(const SMaterialData & aMaterialData);

	private:
		CShape* CreateShape(physx::PxGeometry& aGeometry, const SMaterialData & aMaterialData);
		CShape* CreateShape(physx::PxTriangleMesh& aMesh, const SMaterialData & aMaterialData);

		CShape* CreateShape(physx::PxConvexMesh& aMesh, const SMaterialData & aMaterialData);

	private:
		physx::PxPhysics* myPxPhysics;
		physx::PxDefaultCpuDispatcher* myDispatcher;
		physx::PxMaterial* myDefaultMaterial;

		CSimulationEventCallback* myEventCallback;

	};
}
