#include "stdafx.h"
#include "LoadTumbleweed.h"
#include "TumbleweedFactory.h"
#include "GameObject.h"

int LoadTumbleweed(KLoader::SLoadedComponentData someData)
{
	int id = KLoader::CKevinLoader::GetInstance().GetCurrentObjectIndex();
	CGameObject* parent = reinterpret_cast<CGameObject*>(CComponentManager::GetInstance().GetComponent(id));
	return CTumbleweedFactory::GetInstance()->CreateNewTumbleweed(parent->GetWorldPosition());
}
