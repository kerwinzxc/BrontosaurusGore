#pragma once
#include "GeometryBuffer.h"

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

	struct SCameraStruct
	{
		CU::Matrix44f prjInv;
		CU::Matrix44f camTW;
	};
public:
	CDeferredRenderer();
	~CDeferredRenderer();

	void DoRenderQueue(CRenderer & aRenderer);

	void AddRenderMessage(SRenderMessage* aRenderMessage);
	void UpdateCameraBuffer(const CU::Matrix44f & aCameraSpace, const CU::Matrix44f & aProjectionInverse);
	void SetGeometryBuffer(CGeometryBuffer& myGbuffer);

	void DoLightingPass(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer);
	ID3D11DepthStencilView* GetDepthStencil();
	ID3D11ShaderResourceView* GetDepthResource();
private:
	void DoAmbientLighting(CFullScreenHelper& aFullscreenHelper);
	void DoDirectLighting(CFullScreenHelper& aFullscreenHelper);

	void RenderDirectionalLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper);
	void RenderPointLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper);

	void RenderSpotLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper);
	void DoHighlight(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer);

	void ActivateIntermediate();
	void SetRMAOSRV();

	void SetCBuffer();


	void InitPointLightModel();
	void DoSSAO(CFullScreenHelper& aFullscreenHelper);

private:
	CU::GrowingArray<SRenderMessage*> myRenderMessages;
	CU::GrowingArray<SRenderMessage*> myLightMessages;
	CU::GrowingArray<int> myBatchedModelIds;

	CLightModel* myLightModel;

	ID3D11Buffer* myProjectionInverseBuffer;
	ID3D11Buffer* myDirectionalLightBuffer;
	ID3D11Buffer* myPointLightBuffer;
	ID3D11Buffer* mySpotLightBuffer;

	CGeometryBuffer myGbuffer;
	
	CRenderPackage myIntermediatePackage;
	
	CDXFramework* myFramework;
	CTexture* mySSAORandomTexture;
	CRenderPackage mySSAOPackage;

#ifdef _ENABLE_RENDERMODES
	enum class ERenderMode
	{
		eDiffuse,
		eNormal,
		eRoughness,
		eMetalness,
		eAO,
		eRMAO,
		eSSAO,
		eIntermediate,
		eHighlight,
		eEmissive
	} myRenderMode;
	CU::InputWrapper* myInputWrapper;
	void HandleInput();
#endif 
};