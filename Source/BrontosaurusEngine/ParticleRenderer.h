#pragma once
#include "RenderPackage.h"

struct SRenderParticlesMessage;

class CParticleRenderer
{
public:
	CParticleRenderer(CRenderer& aRenderer, CFullScreenHelper& aHelper);
	~CParticleRenderer();

	void AddRenderMessage(SRenderParticlesMessage* aMessage);

	void DoRenderQueue(ID3D11DepthStencilView* aDepthStencilView);
	CRenderPackage& GetIntermediatePackage();
private:
	void CreateSurface();
	void DoLight();
	void ToIntermediate();

	void ClearParticleTargets();
	void SetParticleTargets(ID3D11DepthStencilView* aDepthStencilView);
	CRenderer& mySharedRenderer;
	CFullScreenHelper& mySharedHelper;
	CU::GrowingArray<SRenderParticlesMessage*> myParticleMessages;

	struct
	{
		CRenderPackage diffuse;
		CRenderPackage normal;
		CRenderPackage RMAO;
	}myParticleGBuffer;
	CRenderPackage myInteremediate;
	CRenderPackage myProcessed;
};

