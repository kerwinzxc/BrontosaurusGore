#pragma once

#include "../CommonUtilities/matrix44.h"
#include "../CommonUtilities/Vector2.h"
#include "../CommonUtilities/Camera.h"

#include "../GUI/GUIPixelConstantBuffer.h"
#include "RenderPackage.h"
#include "FullScreenHelper.h"
#include "BufferStructs.h"
#include "Lights.h"
#include "ParticleEmitter.h"
#include "Colour.h"
#include "../GUI/GUIElement.h"

struct ID3D11RenderTargetView;

class CSkybox;
class CModelInstance;
class CModel;
class CSprite;
class CText;
class CText;
class CPointLightInstance;
class CParticleEmitter;
class CStreakEmitterInstance;

struct SPixelConstantBuffer;
struct SParticle;

enum class eRasterizerState : int;
enum class eBlendState : int;
enum class eDepthStencilState : int;
enum class eSamplerState : int;
enum class eAlignment : int;


typedef unsigned int ParticleEmitterID;

namespace CU
{
	class Camera;
}

namespace Lights
{
	struct SDirectionalLight;
}


struct SRenderMessage
{
	enum class eRenderMessageType
	{
		eSetCamera,
		eRenderCameraQueue,
		eRenderModel,
		eRenderModelDepth,
		eRenderGUIModel,
		eRenderSprite,
		eChangeStates,
		eRenderSkybox,
		eRenderParticles,
		eRenderStreak,
		eRenderFire,
		eRenderText,
		eClearRenderPackage,
		eActivateRenderTo,
		eActivateRenderPackage,
		eRenderFullscreenEffect,
		eRenderDebugObjs,
		eRenderToInterediate,
		eSetShadowBuffer,
		eRenderLineBuffer,
		eRenderNavMesh,
		eSetRTV,
		eSetCubemapResource,
		eClear,
		eRenderModelDeferred,
		eRenderModelShadow,
		eRenderDirectionalLight,
		eRenderPointLight,
		eRenderSpotLight,
		eRenderBar,
		eCreateGuiElement,
		eRenderToGui,
	};

	SRenderMessage(const eRenderMessageType aRenderMessageType);
	virtual ~SRenderMessage();

	eRenderMessageType myType;

};

struct SRenderDirectionalLight : SRenderMessage
{
	SRenderDirectionalLight();
	Lights::SDirectionalLight directionalLight;
};

struct SRenderPointLight : SRenderMessage
{
	SRenderPointLight();
	Lights::SPointLight pointLight;
};

struct SRenderSpotLight : SRenderMessage
{
	SRenderSpotLight();
	Lights::SSpotLight spotLight;
};

struct SSetCubemapResource : SRenderMessage
{
	SSetCubemapResource();
	//Incr refcount
	ID3D11ShaderResourceView* mySRV;
};


struct SClear : SRenderMessage
{
	SClear();
	//Incr refcount
	ID3D11RenderTargetView* myRTV;
	ID3D11DepthStencilView* myDSV;
};


struct SSetRTVMessage : SRenderMessage
{
	SSetRTVMessage();
	//Incr refcount
	D3D11_VIEWPORT* myViewport;
	ID3D11RenderTargetView* myRTV;
};

struct SActivateRenderToMessage : SRenderMessage
{
	SActivateRenderToMessage();
};


struct SClearRenderPackageMessage : SRenderMessage
{
	SClearRenderPackageMessage();
	CRenderPackage myRenderPackage;
};


struct SRenderNavmeshMessage : SRenderMessage
{
	SRenderNavmeshMessage();
	CU::Matrix44f myTransformation;
	CModel* myModel;
};


struct SRenderFullscreenEffectMessage : SRenderMessage
{
	SRenderFullscreenEffectMessage();

	CU::Vector4f myRect;

	bool myFirstUseDepthResource;
	bool mySecondUseDepthResource;

	// If effect with only one renderpackage set only first
	CRenderPackage myFirstPackage;
	CRenderPackage mySecondPackage;

	CFullScreenHelper::eEffectType myEffectType;

};

struct SActivateRenderPackageMessage : SRenderMessage
{
	SActivateRenderPackageMessage();
	CRenderPackage myRenderPackage;
	CRenderPackage mySecondRenderPackage;
	bool useSecondPackage;
};

struct SRenderToIntermediate : SRenderMessage
{
	SRenderToIntermediate();
	CRenderPackage myRenderPackage;
	CU::Vector4f myRect;
	bool useDepthResource;
};

struct SRenderCameraQueueMessage : SRenderMessage
{
	~SRenderCameraQueueMessage() override;
	//mebe put SetCameraMessage in here?
	SRenderCameraQueueMessage();
	CU::Camera myCamera;
	CRenderPackage CameraRenderPackage;
	CU::GrowingArray < SRenderMessage*, unsigned int, false> CameraRenderQueue;
	//CU::GrowingArray < SRenderMessage*, unsigned int, false> DeferredCameraRenderQueue;
	//CDeferredRenderer myDeferredRenderer;
	bool RenderDepth;
};

struct SRenderModelMessage : SRenderMessage
{
	SRenderModelMessage();
	SForwardRenderModelParams myRenderParams;
	int myModelID;
};

struct SRenderModelDeferredMessage : SRenderMessage
{
	SRenderModelDeferredMessage();
	SDeferredRenderModelParams myRenderParams;
	int myModelID;
};

struct SRenderModelShadowMessage : SRenderMessage
{
	SRenderModelShadowMessage();
	SShadowRenderModelParams myRenderParams;
	int myModelID;
};

struct SSetShadowBuffer : SRenderMessage
{
	SSetShadowBuffer();

	CRenderPackage myShadowBuffer;
	CU::Matrix44f myCameraTransformation;
	CU::Matrix44f myCameraProjection;
};


struct SRenderModelDepthMessage : SRenderModelMessage
{
	SRenderModelDepthMessage();
};

struct SRenderGUIModelMessage : SRenderMessage
{
	SRenderGUIModelMessage();

	CU::Matrix44f myToWorld;
	SPixelConstantBuffer myPixelConstantBufferStruct;
	int myModelID;

};

struct SRenderParticlesMessage : SRenderMessage
{
	SRenderParticlesMessage();
	~SRenderParticlesMessage();
	ParticleEmitterID particleEmitter;
	CU::GrowingArray<SParticle, unsigned int, false> particleList;
	CU::Matrix44f toWorld;
	CParticleEmitter::RenderMode renderMode = CParticleEmitter::RenderMode::eMetaBall;
};

struct SRenderStreakMessage : SRenderMessage
{
	SRenderStreakMessage();
	CStreakEmitterInstance* streakEmitter;
};

struct SRenderFireMessage : SRenderMessage
{
	SRenderFireMessage();
	CU::Matrix44f myToWorldMatrix;
	int myFireID;
};

struct SRenderSpriteMessage : SRenderMessage
{
	SRenderSpriteMessage();
	CU::Vector2f myPosition;
	CU::Vector2f mySize;
	CU::Vector2f myPivot;
	CU::Vector4f myRect;
	CU::Vector4f myColor;
	CSprite* mySprite;
};

struct SRenderSkyboxMessage : SRenderMessage
{
	SRenderSkyboxMessage();
	SRenderSkyboxMessage(const SRenderSkyboxMessage& aCopy) = delete;
	~SRenderSkyboxMessage();
	CSkybox* mySkybox;
};

struct SSetCameraMessage : SRenderMessage
{
	SSetCameraMessage();
	CU::Camera myCamera;
};


struct SChangeStatesMessage : SRenderMessage
{
	SChangeStatesMessage();
	eRasterizerState myRasterizerState;
	eBlendState myBlendState;
	eDepthStencilState myDepthStencilState;
	eSamplerState mySamplerState;
};


struct SRenderTextMessage : SRenderMessage
{
	SRenderTextMessage();
	CU::Vector4f myColor;
	CU::GrowingArray<std::wstring> myStrings;
	CU::Vector2f myPosition;
	CText *myText;
	CU::Vector2f myLineHeight;
	eAlignment myAlignement;
};


struct SRenderLineBuffer : SRenderMessage
{
	SRenderLineBuffer(const CU::GrowingArray<char, unsigned int, false>& aLineBuffer);
	CU::GrowingArray<char, unsigned int, false> myLineBuffer;
};

struct SRenderBarMessage : SRenderMessage
{
	SRenderBarMessage(): SRenderMessage(eRenderMessageType::eRenderBar), myCurrentLevel(0) {}

	CU::Vector4f myRect;
	CU::Colour myFullColour;
	CU::Colour myEmptyColour;
	CU::Colour myBackgroundColour;
	float myCurrentLevel;
};

struct SCreateOrClearGuiElement : SRenderMessage
{
	SCreateOrClearGuiElement(const std::wstring& anElementName, const SGUIElement& aGUIElement, const CU::Vector2ui aPixelSize);

	const std::wstring myElementName;
	const SGUIElement myGuiElement;
	const CU::Vector2ui myPixelSize;
};

struct SRenderToGUI : SRenderMessage
{
	SRenderToGUI(const std::wstring& anElementName, SRenderMessage * const aRenderMessage);
	~SRenderToGUI();

	const std::wstring myElementName;
	SRenderMessage* myRenderMessage;
};