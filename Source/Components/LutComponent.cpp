#include "stdafx.h"
#include "LutComponent.h"
#include "ComponentManager.h"

#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/Renderer.h"
#include "../BrontosaurusEngine/RenderMessages.h"
#include "../BrontosaurusEngine/ELUTType.h"

CLutComponent::CLutComponent()
{
	if (CComponentManager::GetInstancePtr())
	{
		CComponentManager::GetInstance().RegisterComponent(this);
	}
}

CLutComponent::~CLutComponent()
{
}

void CLutComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eTookDamage:
		SendHurtLutMessage();
		break;
	}
}

void CLutComponent::SendHurtLutMessage()
{
	CRenderer& renderer = RENDERER;

	SLutFadeColorGrade message;
	message.myFadeTime = 0.2f;
	message.myFadeTo = ELUTType::eHurt;
	message.myInterrupt = true;

	renderer.AddRenderMessage(new SLutFadeColorGrade(message));

	message.myFadeTime = 0.4f;
	message.myFadeTo = ELUTType::eDefault;
	message.myInterrupt = false;

	renderer.AddRenderMessage(new SLutFadeColorGrade(message));
}
