#pragma once

namespace KLoader
{
	struct SLoadedComponentData;
}

int LoadSphereCollider(KLoader::SLoadedComponentData someData);
int LoadCapsuleCollider(KLoader::SLoadedComponentData someData);
int LoadBoxCollider(KLoader::SLoadedComponentData someData);
int LoadMeshCollider(KLoader::SLoadedComponentData someData);

int LoadRigidBody(KLoader::SLoadedComponentData someData);
int LoadCharacterController(KLoader::SLoadedComponentData someData);

int LoadSphereColliderServer(KLoader::SLoadedComponentData someData);
int LoadCapsuleColliderServer(KLoader::SLoadedComponentData someData);
int LoadBoxColliderServer(KLoader::SLoadedComponentData someData);
int LoadMeshColliderServer(KLoader::SLoadedComponentData someData);

int LoadRigidBodyServer(KLoader::SLoadedComponentData someData);
int LoadCharacterControllerServer(KLoader::SLoadedComponentData someData);
