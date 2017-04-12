#include "stdafx.h"
#include "RenderMessages.h"
#include "Renderer.h"
#include "Skybox.h"
#include "EmitterData.h"
#include "ELUTType.h"

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


SRenderCameraQueueMessage::SRenderCameraQueueMessage()
	: SRenderMessage(eRenderMessageType::eRenderCameraQueue)
{
	RenderDepth = false;
}

SRenderCameraQueueMessage::~SRenderCameraQueueMessage()
{
	//CameraRenderQueue.DeleteAll();
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
	useSecondPackage = false;
	unbindShadowBuffer = false;
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

SSetRTVMessage::SSetRTVMessage()
	: SRenderMessage(SRenderMessage::eRenderMessageType::eSetRTV)
{
	myRTV = nullptr;
}

SClear::SClear()
	: SRenderMessage(SRenderMessage::eRenderMessageType::eClear)
{
	myRTV = nullptr;
	myDSV = nullptr;
}

SSetCubemapResource::SSetCubemapResource()
	: SRenderMessage(SRenderMessage::eRenderMessageType::eSetCubemapResource)
{
	mySRV = nullptr;
}

SRenderModelDeferredMessage::SRenderModelDeferredMessage()
	:SRenderMessage(SRenderMessage::eRenderMessageType::eRenderModelDeferred)
{
	myModelID = -1;
}

SRenderDirectionalLight::SRenderDirectionalLight()
	: SRenderMessage(SRenderMessage::eRenderMessageType::eRenderDirectionalLight)
{
}

SRenderPointLight::SRenderPointLight()
	: SRenderMessage(SRenderMessage::eRenderMessageType::eRenderPointLight)
{

}

SRenderSpotLight::SRenderSpotLight()
	: SRenderMessage(SRenderMessage::eRenderMessageType::eRenderSpotLight)
{

}

SRenderModelShadowMessage::SRenderModelShadowMessage()
	: SRenderMessage(SRenderMessage::eRenderMessageType::eRenderModelShadow)
{
	myModelID = -1;
}

SCreateOrClearGuiElement::SCreateOrClearGuiElement(const std::wstring& anElementName, const SGUIElement& aGUIElement, const CU::Vector2ui aPixelSize) :
	SRenderMessage(eRenderMessageType::eCreateGuiElement),
	myElementName(anElementName),
	myGuiElement(aGUIElement),
	myPixelSize(aPixelSize)
{
}

SRenderToGUI::SRenderToGUI(const std::wstring& anElementName, SRenderMessage* const aRenderMessage): 
	SRenderMessage(eRenderMessageType::eRenderToGui), 
	myElementName(anElementName), 
	myRenderMessage(aRenderMessage)
{
}

SRenderModelInstancedMessage::SRenderModelInstancedMessage()
	: SRenderMessage(eRenderMessageType::eRenderModelInstanced)
{
	myModelID = -1;
}

SRenderModelBatches::SRenderModelBatches()
	: SRenderMessage(eRenderMessageType::eRenderModelBatches)
{
	myRenderToDepth = false;
}

SRenderToGUI::~SRenderToGUI()
{
	delete myRenderMessage;
}

SLutFadeColorGrade::SLutFadeColorGrade()
	: SRenderMessage(eRenderMessageType::eLUTFADECOLORGRADE)
{
	myFadeTo = myFadeFrom = ELUTType::eDefault;
	myFadeTime = 1.f;
}
