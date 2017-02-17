#include "stdafx.h"
#include "RenderMessages.h"
#include "Renderer.h"
#include "Skybox.h"
#include "EmitterData.h"

SRenderMessage::SRenderMessage(const eRenderMessageType aRenderMessageType)
	: myType(aRenderMessageType)
{
}

SRenderMessage::~SRenderMessage()
{
}

SRenderModelMessage::SRenderModelMessage()
	: SRenderMessage(eRenderMessageType::eRenderModel)
{
}

SRenderGUIModelMessage::SRenderGUIModelMessage()
	: SRenderMessage(eRenderMessageType::eRenderGUIModel)
{
}

SRenderParticlesMessage::SRenderParticlesMessage()
	: SRenderMessage(eRenderMessageType::eRenderParticles)
{
}

SRenderParticlesMessage::~SRenderParticlesMessage()
{
}

SRenderStreakMessage::SRenderStreakMessage()
	: SRenderMessage(eRenderMessageType::eRenderStreak)
{
}

SRenderFireMessage::SRenderFireMessage()
	: SRenderMessage(eRenderMessageType::eRenderFire)
{
}

SRenderSpriteMessage::SRenderSpriteMessage()
	: SRenderMessage(eRenderMessageType::eRenderSprite)
{
}

SRenderSkyboxMessage::SRenderSkyboxMessage()
	: SRenderMessage(eRenderMessageType::eRenderSkybox)
	, mySkybox(nullptr)
{
}

SRenderSkyboxMessage::~SRenderSkyboxMessage()
{
	if (mySkybox)
	{
		if (mySkybox->DecRef() <= 0)
		{
			SAFE_DELETE(mySkybox);
		}
	}
}

SSetCameraMessage::SSetCameraMessage()
	: SRenderMessage(eRenderMessageType::eSetCamera)
{
}

SChangeStatesMessage::SChangeStatesMessage()
	: SRenderMessage(eRenderMessageType::eChangeStates)
{
	//mySamplerState = eSamplerState::eSize;
}

SRenderTextMessage::SRenderTextMessage()
	: SRenderMessage(eRenderMessageType::eRenderText)
{
}

SRenderModelDepthMessage::SRenderModelDepthMessage()
{
	myType = eRenderMessageType::eRenderModelDepth;
}

SRenderCameraQueueMessage::~SRenderCameraQueueMessage()
{
	CameraRenderQueue.DeleteAll();
}

SRenderCameraQueueMessage::SRenderCameraQueueMessage()
	: SRenderMessage(eRenderMessageType::eRenderCameraQueue)
{
	RenderDepth = false;
}

SRenderFullscreenEffectMessage::SRenderFullscreenEffectMessage()
	:SRenderMessage(eRenderMessageType::eRenderFullscreenEffect)
{
	myFirstUseDepthResource = false;
	mySecondUseDepthResource = false;
}

SActivateRenderPackageMessage::SActivateRenderPackageMessage()
	: SRenderMessage(eRenderMessageType::eActivateRenderPackage)
{

}

SRenderToIntermediate::SRenderToIntermediate()
	: SRenderMessage(SRenderMessage::eRenderMessageType::eRenderToInterediate),
	useDepthResource(0)
{
	myRect = CU::Vector4f::Zero;
}

SSetShadowBuffer::SSetShadowBuffer()
	: SRenderMessage(eRenderMessageType::eSetShadowBuffer)
{
}

SRenderNavmeshMessage::SRenderNavmeshMessage()
	: SRenderMessage(SRenderMessage::eRenderMessageType::eRenderNavMesh)
{
	myModel = nullptr;
}

SRenderLineBuffer::SRenderLineBuffer(const CU::GrowingArray<char, unsigned int, false>& aLineBuffer)
	: SRenderMessage(eRenderMessageType::eRenderLineBuffer)
	, myLineBuffer(aLineBuffer)
{
}

SClearRenderPackageMessage::SClearRenderPackageMessage()
	: SRenderMessage(eRenderMessageType::eClearRenderPackage)
{
}

SActivateRenderToMessage::SActivateRenderToMessage()
	: SRenderMessage(SRenderMessage::eRenderMessageType::eActivateRenderTo)
{
}
