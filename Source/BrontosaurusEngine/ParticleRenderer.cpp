#include "stdafx.h"
#include "ParticleRenderer.h"
#include "ParticleEmitter.h"
#include "RenderMessages.h"
#include "ParticleEmitterManager.h"


CParticleRenderer::CParticleRenderer(CRenderer& aRenderer, CFullScreenHelper& aHelper) : mySharedRenderer(aRenderer), mySharedHelper(aHelper)
{

	CU::Vector2ui windowSize = CEngine::GetInstance()->GetWindowSize();
	myParticleGBuffer.diffuse.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myParticleGBuffer.normal.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myParticleGBuffer.RMAO.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myParticleMessages.Init(10);
}


CParticleRenderer::~CParticleRenderer()
{
}

void CParticleRenderer::AddRenderMessage(SRenderParticlesMessage* aMessage)
{
	myParticleMessages.Add(aMessage);
}

void CParticleRenderer::DoRenderQueue(ID3D11DepthStencilView* aDepthStencilView)
{
	ClearParticleTargets();
	SetParticleTargets(aDepthStencilView);
	for (int i = 0; i < myParticleMessages.Size(); ++i)
	{
		SRenderParticlesMessage* msg = static_cast<SRenderParticlesMessage*>(myParticleMessages[i]);
		CParticleEmitter* emitter = ENGINE->GetParticleEmitterManager().GetParticleEmitter(msg->particleEmitter);
		if (emitter == nullptr)	break;

		emitter->Render(msg->toWorld, msg->particleList);
	}
	myParticleMessages.RemoveAll();
}

void CParticleRenderer::ClearParticleTargets()
{
	myParticleGBuffer.diffuse.Clear();
	myParticleGBuffer.normal.Clear();
	myParticleGBuffer.RMAO.Clear();
}

void CParticleRenderer::SetParticleTargets(ID3D11DepthStencilView* aDepthStencilView)
{
	ID3D11RenderTargetView* rtvs[3];
	rtvs[0] = myParticleGBuffer.diffuse.GetRenderTargetView();
	rtvs[1] = myParticleGBuffer.normal.GetRenderTargetView();
	rtvs[2] = myParticleGBuffer.RMAO.GetRenderTargetView();

	DEVICE_CONTEXT->OMSetRenderTargets(3, rtvs, aDepthStencilView);
}
