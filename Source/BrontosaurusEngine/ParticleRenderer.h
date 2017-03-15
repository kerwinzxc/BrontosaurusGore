#pragma once
#include "RenderPackage.h"

struct SRenderParticlesMessage;

class CParticleRenderer
{
public:
	CParticleRenderer();
	~CParticleRenderer();

	void AddRenderMessage(SRenderParticlesMessage* aMessage);

	
	void DoRenderQueue(ID3D11DepthStencilView* aDepthStencilView);
private:
	struct
	{
		CRenderPackage diffuse;
		CRenderPackage normal;
		CRenderPackage RMAO;
	}myParticleGBuffer;
	void ClearParticleTargets();
	void SetParticleTargets(ID3D11DepthStencilView* aDepthStencilView);
	CU::GrowingArray<SRenderParticlesMessage*> myParticleMessages;
};

