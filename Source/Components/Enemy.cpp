#include "stdafx.h"
#include "Enemy.h"

CGameObject* Component::CEnemy::myPlayerObject = nullptr;

Component::CEnemy::CEnemy(): myHealth(0), mySpeed(0), myDetectionRange2(0), myStartAttackRange2(0), myStopAttackRange2(0)
{
	myIsDead = false;
}


Component::CEnemy::~CEnemy()
{
}

void Component::CEnemy::Update(const CU::Time& aDeltaTime)
{
	if(myPlayerObject != nullptr)
	{
		if (myIsDead == false)
		{
			const CU::Vector3f playerPosition = myPlayerObject->GetWorldPosition();
			const CU::Vector3f position = GetParent()->GetWorldPosition();
			const CU::Vector3f dif = playerPosition - position;
			const CU::Vector3f difNorm = dif.GetNormalized();
			const float dist2 = dif.Length2();

			if (dist2 < myStartAttackRange2)
			{

			}
			else if (dist2 < myDetectionRange2)
			{
				const CU::Vector3f displ = difNorm * mySpeed * aDeltaTime.GetSeconds();
				GetParent()->Move(displ);


			}
			if (dist2 < myDetectionRange2)
			{
				GetParent()->Face(difNorm);
			}
		}
	}
}

void Component::CEnemy::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eDied:
		myIsDead = true;
		break;
	}
}

void Component::CEnemy::SetPlayer(CGameObject* playerObject)
{
	myPlayerObject = playerObject;
}
