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
