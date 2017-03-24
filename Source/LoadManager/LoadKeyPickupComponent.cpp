#include "stdafx.h"
#include "LoadKeyPickupComponent.h"
#include "KeyPickupComponent.h"
#include "PickupComponentManager.h"

int LoadKeyPickupComponent(KLoader::SLoadedComponentData someData)
{

	short keyID = someData.myData.at("LockID").GetInt();

	CKeyPickupComponent* key = CPickupComponentManager::GetInstance()->CreateKeyPickupComponent(keyID);

	return key->GetId();
}
