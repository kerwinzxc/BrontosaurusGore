#pragma once
#include "RenderPackage.h"


#ifndef _RETAIL_BUILD
#define _ENABLE_RENDERMODES
#endif


struct SRenderMessage;
struct ID3D11Buffer;
class CDXFramework;

class CLightModel;

class CFullScreenHelper;
class CTexture;

#ifdef _ENABLE_RENDERMODES
namespace CU
{
	class InputWrapper;
}
#endif


class CDeferredRenderer
{
	friend CRenderer;
	struct SGBuffer
	{
		CRenderPackage diffuse;
		CRenderPackage normal;
		CRenderPackage RMAO;
		CRenderPackage emissive;
	};

	struct SCameraStruct
	{
		CU::Matrix44f prjInv;
		CU::Matrix44f camTW;
	};

public:
	CDeferredRenderer();
	~CDeferredRenderer();

	void DoRenderMessage(SRenderMessage* aRenderMessage);
	void DoRenderQueue();

	void AddRenderMessage(SRenderMessage* aRenderMessage);
	void UpdateCameraBuffer(const CU::Matrix44f & aCameraSpace, const CU::Matrix44f & aProjectionInverse);
	
	void DoLightingPass(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer);
	ID3D11DepthStencilView* GetDepthStencil();
private:
	void SetRenderTargets();
	void ClearRenderTargets();

	void DoAmbientLighting(CFullScreenHelper& aFullscreenHelper);
	void DoDirectLighting(CFullScreenHelper& aFullscreenHelper);

	void RenderDirectionalLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper);
	void RenderPointLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper);

	void RenderSpotLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper);

	void ActivateIntermediate();
	void ActivateSSAO();
	void SetSRV();
	void SetRMAOSRV();

	void SetCBuffer();

	void InitPointLightModel();
	void DoSSAO(CFullScreenHelper& aFullscreenHelper);


private:
	CU::GrowingArray<SRenderMessage*> myRenderMessages;
	CU::GrowingArray<SRenderMessage*> myLightMessages;
	CU::GrowingArray<SRenderMessage*> myParticleMessages;

	CLightModel* myLightModel;

	ID3D11Buffer* myProjectionInverseBuffer;
	ID3D11Buffer* myDirectionalLightBuffer;
	ID3D11Buffer* myPointLightBuffer;
	ID3D11Buffer* mySpotLightBuffer;

	SGBuffer myGbuffer;
	
	CRenderPackage myIntermediatePackage;
	
	CDXFramework* myFramework;
	CTexture* mySSAORandomTexture;
	CRenderPackage mySSAOPackage;

#ifdef _ENABLE_RENDERMODES
	enum ERenderMode
	{
		eDiffuse,
		eNormal,
		eRoughness,
		eMetalness,
		eAO,
		eSSAO,
		eIntermediate,
	} myRenderMode;
	CU::InputWrapper* myInputWrapper;
	void HandleInput();
#endif 
};

