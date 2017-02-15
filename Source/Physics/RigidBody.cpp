#include "stdafx.h"
#include "RigidBody.h"
//#include "../Components/Components/Component.h"
#include "../Components/Components/GameObject/GameObject.h"
#include "RigidBodyComponent.h"


CRigidBody::CRigidBody() :CRigidBody(0)
{
	myVelocity = CU::Vector3f::Zero;
}

CRigidBody::CRigidBody(float aMass):myMass(aMass), myComponent(nullptr)
{
	
}

CRigidBody::~CRigidBody()
{
}

void CRigidBody::Update()
{
	myVelocity = myComponent->GetParent()->GetToWorldTransform().GetPosition() - myPrevoiusPosition;
	myVelocity -= myPrevoiusImpulse;
	myPrevoiusImpulse = CU::Vector3f::Zero;
	myPrevoiusImpulse = myComponent->GetParent()->GetToWorldTransform().GetPosition();
}

void CRigidBody::AddImpulse(const CU::Vector3f& anImpulse)
{
	myPrevoiusImpulse += anImpulse;
}

void CRigidBody::SetComponent(CRigidBodyComponent* aComponent)
{
	myComponent = aComponent;
}
