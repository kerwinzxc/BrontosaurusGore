#pragma once

#include "../CommonUtilities/matrix44.h"
#include "../CommonUtilities/DynamicString.h"
#include "../CommonUtilities/Vector2.h"
#include "../CommonUtilities/Camera.h"

#include "../GUI/GUIPixelConstantBuffer.h"
#include "RenderPackage.h"
#include "FullScreenHelper.h"
#include "BufferStructs.h"



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
	};

	SRenderMessage(const eRenderMessageType aRenderMessageType);
	virtual ~SRenderMessage();

	eRenderMessageType myType;

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
	bool RenderDepth;
};

struct SRenderModelMessage : SRenderMessage
{
	SRenderModelMessage();
	SRenderModelParams myRenderParams;
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
	CU::GrowingArray<CU::DynamicString> myStrings;
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
