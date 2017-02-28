#pragma once
#include "RenderPackage.h"

struct SRenderMessage;
struct ID3D11Buffer;
class CDXFramework;

class CFullScreenHelper;


class CDeferredRenderer
{
	friend CRenderer;
	struct SGBuffer
	{
		CRenderPackage diffuse;
		CRenderPackage normal;
		CRenderPackage roughnessMetalnessAO;
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
private:
	void SetRenderTargets();
	void ClearRenderTargets();

	void DoAmbientLighting(CFullScreenHelper& aFullscreenHelper);
	void DoDirectLighting(CFullScreenHelper& aFullscreenHelper);

	void RenderDirectionalLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper);
	void RenderPointLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper);

	void RenderSpotLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper);

	void ActivateIntermediate();
	void SetSRV();
	void SetCBuffer();

private:
	CU::GrowingArray<SRenderMessage*> myRenderMessages;
	CU::GrowingArray<SRenderMessage*> myLightMessages;


	ID3D11Buffer* myProjectionInverseBuffer;
	ID3D11Buffer* myDirectionalLightBuffer;
	ID3D11Buffer* myPointLightBuffer;
	ID3D11Buffer* mySpotLightBuffer;

	SGBuffer myGbuffer;
	CRenderPackage myIntermediatePackage;
	CDXFramework* myFramework;
};

