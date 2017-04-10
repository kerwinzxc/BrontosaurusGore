#pragma once
#include "RenderPackage.h"
#include "LightModel.h"
#include "EmitterData.h"

struct SRenderMessage;

class CParticleRenderer
{
	struct SCameraStruct
	{
		CU::Matrix44f prjInv;
		CU::Matrix44f camTW;
	};

public:

	
	CParticleRenderer(CRenderer& aRenderer, CFullScreenHelper& aHelper);
	~CParticleRenderer();

	void AddRenderMessage(SRenderMessage* aMessage);


	void DoRenderQueue(ID3D11DepthStencilView* aDepthStencilView,ID3D11ShaderResourceView* aDepthResourceView);
	CRenderPackage& GetIntermediatePackage();

	
private:
	void SetBlendParticlesState();
	void InitPointLightModel();

	void SortParticles(CU::GrowingArray<SParticle, unsigned, false> &aGrowings);

	void UpdateCameraBuffer(const CU::Matrix44f& aMatrix44, const CU::Matrix44f& aProjectionInverse);

	
	void SetSRV();
	void SetCBuffer();
	void DoAmbientLighting();
	void RenderDirectionalLight(SRenderMessage* aRenderMessage);
	void RenderPointLight(SRenderMessage* aRenderMessage);
	void RenderSpotLight(SRenderMessage* aRenderMessage);
	void DoDirectLighting();
	void DoLight();
	void ToIntermediate();

	void ClearParticleTargets();
	void SetParticleTargets(ID3D11DepthStencilView* aDepthStencilView);
	void SetSurfaceTargets(ID3D11DepthStencilView* aDepthStencilView);

	void CreateSurface(ID3D11DepthStencilView* aDepthStencilView);
	CRenderer& mySharedRenderer;
	CFullScreenHelper& mySharedHelper;
	CU::GrowingArray<SRenderMessage*> myParticleMessages;
	CU::GrowingArray<SRenderMessage*> myLightMessages;

	struct
	{
		CRenderPackage diffuse;
		CRenderPackage normal;
		CRenderPackage RMAO;
		CRenderPackage alpha;

		CRenderPackage surfaceDiffuse;
		CRenderPackage surfaceNormal;
		CRenderPackage surfaceRMAO;
		CRenderPackage sulfaceAlpha;
	}myParticleGBuffer;
	CRenderPackage myInteremediate;
	CRenderPackage myTempIntermediate;
	CRenderPackage myProcessed;

	ID3D11Buffer* myProjectionInverseBuffer;
	ID3D11Buffer* myDirectionalLightBuffer;
	ID3D11Buffer* myPointLightBuffer;
	ID3D11Buffer* mySpotLightBuffer;
	ID3D11ShaderResourceView* myDepthStencilResourceToUse;
	CLightModel* myLightModel;
};

