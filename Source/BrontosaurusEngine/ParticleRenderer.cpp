#include "stdafx.h"
#include "ParticleRenderer.h"
#include "ParticleEmitter.h"
#include "RenderMessages.h"
#include "ParticleEmitterManager.h"
#include "ConstBufferTemplate.h"
#include "Renderer.h"
#include "FBXLoader.h"
#include "LightModel.h"
#include "ShaderManager.h"

CParticleRenderer::CParticleRenderer(CRenderer& aRenderer, CFullScreenHelper& aHelper) : mySharedRenderer(aRenderer), mySharedHelper(aHelper)
{
	CU::Vector2ui windowSize = CEngine::GetInstance()->GetWindowSize();
	myParticleGBuffer.diffuse.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myParticleGBuffer.normal.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myParticleGBuffer.RMAO.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);

	myParticleGBuffer.surfaceDiffuse.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myParticleGBuffer.surfaceNormal.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myParticleGBuffer.surfaceRMAO.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);

	myProcessed.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myInteremediate.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myParticleMessages.Init(10);
	myLightMessages.Init(10);

	
	SCameraStruct temp;
	myProjectionInverseBuffer = BSR::CreateCBuffer<SCameraStruct>(&temp);

	Lights::SDirectionalLight directionalLight;
	myDirectionalLightBuffer = BSR::CreateCBuffer<Lights::SDirectionalLight>(&directionalLight);

	Lights::SPointLight pointLight;
	myPointLightBuffer = BSR::CreateCBuffer<Lights::SPointLight>(&pointLight);

	Lights::SSpotLight spotLight;
	mySpotLightBuffer = BSR::CreateCBuffer<Lights::SSpotLight>(&spotLight);

	InitPointLightModel();
}

CParticleRenderer::~CParticleRenderer()
{
}

void CParticleRenderer::AddRenderMessage(SRenderMessage* aMessage)
{
	switch (aMessage->myType)
	{
	case SRenderMessage::eRenderMessageType::eRenderSpotLight:
	case SRenderMessage::eRenderMessageType::eRenderPointLight:
	case SRenderMessage::eRenderMessageType::eRenderDirectionalLight:
		myLightMessages.Add(aMessage);
		break;
	case SRenderMessage::eRenderMessageType::eRenderParticles:
		myParticleMessages.Add(aMessage);
		break;
	}
}

void CParticleRenderer::DoRenderQueue(ID3D11DepthStencilView* aDepthStencilView, ID3D11ShaderResourceView* aDepthResourceView)
{
	myDepthStencilResourceToUse = aDepthResourceView;
	myInteremediate.Clear();
	UpdateCameraBuffer(mySharedRenderer.GetCamera().GetTransformation(), mySharedRenderer.GetCamera().GetProjectionInverse());
	for (int i = 0; i < myParticleMessages.Size(); ++i)
	{
		SetBlendParticlesState();
		ClearParticleTargets();
		SetParticleTargets(aDepthStencilView);
		SRenderParticlesMessage* msg = static_cast<SRenderParticlesMessage*>(myParticleMessages[i]);
		CParticleEmitter* emitter = ENGINE->GetParticleEmitterManager().GetParticleEmitter(msg->particleEmitter);
		if (emitter == nullptr)	break;

		//SortParticles(msg->particleList);
		emitter->Render(msg->toWorld, msg->particleList, msg->renderMode);

		if(msg->renderMode == CParticleEmitter::RenderMode::eMetaBall)
		{
			CreateSurface(aDepthStencilView);
		}
		if(msg->renderMode != CParticleEmitter::RenderMode::eBillboard)
		{
			DoLight();
		}
		ToIntermediate();
	}
	myParticleMessages.RemoveAll();
	myLightMessages.RemoveAll();
}

CRenderPackage& CParticleRenderer::GetIntermediatePackage()
{
	return myInteremediate;
}

void CParticleRenderer::SetBlendParticlesState()
{

	SChangeStatesMessage changeStateMessage = {};
	changeStateMessage.myRasterizerState = eRasterizerState::eDefault;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eReadOnly;
	changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	mySharedRenderer.SetStates(&changeStateMessage);
}

void CParticleRenderer::InitPointLightModel()
{
	myLightModel = new CLightModel();

	CFBXLoader modelLoader;
	CLoaderModel* model = modelLoader.LoadModel("Models/lightmeshes/sphere.fbx");
	CLoaderMesh* mesh = model->myMeshes[0];

	unsigned int shaderBlueprint = mesh->myShaderType;
	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/Deferred/deferred_vertex.fx", shaderBlueprint);
	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/Deferred/deferred_pointlight.fx", shaderBlueprint);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/Deferred/deferred_vertex.fx", shaderBlueprint);


	CEffect* effect = new CEffect(vertexShader, pixelShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myLightModel->Init(effect, mesh);
}

void CParticleRenderer::SortParticles(CU::GrowingArray<SParticle, unsigned, false>& aGrowings)
{
	const CU::Matrix44f cameraInv = mySharedRenderer.GetCamera().GetInverse();
	const CU::Vector4f cameraPos = CU::Vector4f(mySharedRenderer.GetCamera().GetPosition());
	/*for(int i = 0; i < aGrowings.Size(); ++i)
	{
		aGrowings[i].position = aGrowings[i].position * cameraInv;
	}*/

	aGrowings.QuickSort([cameraPos](SParticle left, SParticle right)->bool
	{
		return (cameraPos - left.position).Length2() < (cameraPos - right.position).Length2();
	});

	/*const CU::Matrix44f camera = mySharedRenderer.GetCamera().GetTransformation();

	for (int i = 0; i < aGrowings.Size(); ++i)
	{
		aGrowings[i].position = aGrowings[i].position * camera;
	}*/
}

void CParticleRenderer::UpdateCameraBuffer(const CU::Matrix44f& aMatrix44, const CU::Matrix44f& aProjectionInverse)
{
	SCameraStruct updatedMatrices;
	updatedMatrices.prjInv = aProjectionInverse;
	updatedMatrices.camTW = aMatrix44;

	BSR::UpdateCBuffer<SCameraStruct>(myProjectionInverseBuffer, &updatedMatrices);
}



void CParticleRenderer::SetSRV()
{
	ID3D11ShaderResourceView* srvs[5];
	srvs[0] = myParticleGBuffer.diffuse.GetResource();
	srvs[1] = myParticleGBuffer.normal.GetResource();
	srvs[2] = myParticleGBuffer.RMAO.GetResource();
	srvs[3] = nullptr;
	srvs[4] = myDepthStencilResourceToUse;
	DEVICE_CONTEXT->PSSetShaderResources(1, 5, srvs);
}

void CParticleRenderer::SetCBuffer()
{
	DEVICE_CONTEXT->PSSetConstantBuffers(1, 1, &myProjectionInverseBuffer);
}

void CParticleRenderer::DoAmbientLighting()
{

	mySharedHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredAmbient);
}

void CParticleRenderer::RenderDirectionalLight(SRenderMessage * aRenderMessage)
{
	SRenderDirectionalLight* msg = static_cast<SRenderDirectionalLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SDirectionalLight>(myDirectionalLightBuffer, &msg->directionalLight);
	DEVICE_CONTEXT->PSSetConstantBuffers(2, 1, &myDirectionalLightBuffer);
	mySharedHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredDirectional);
}

void CParticleRenderer::RenderPointLight(SRenderMessage* aRenderMessage)
{
	SRenderPointLight* msg = static_cast<SRenderPointLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SPointLight>(myPointLightBuffer, &msg->pointLight);
	DEVICE_CONTEXT->PSSetConstantBuffers(2, 1, &myPointLightBuffer);
	CU::Matrix44f pointLightTransformation;
	pointLightTransformation.myPosition = msg->pointLight.position;
	float scale = msg->pointLight.range;
	pointLightTransformation.Scale({ scale, scale, scale });
	myLightModel->Render(pointLightTransformation);
}

void CParticleRenderer::RenderSpotLight(SRenderMessage* aRenderMessage)
{
	SRenderSpotLight* msg = static_cast<SRenderSpotLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SSpotLight>(mySpotLightBuffer, &msg->spotLight);
	DEVICE_CONTEXT->PSSetConstantBuffers(2, 1, &mySpotLightBuffer);
	mySharedHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredSpotLight);
}

void CParticleRenderer::DoDirectLighting()
{
	for (SRenderMessage* renderMessage : myLightMessages)
	{
		switch (renderMessage->myType)
		{
		case SRenderMessage::eRenderMessageType::eRenderDirectionalLight:
			RenderDirectionalLight(renderMessage);
			break;
		case SRenderMessage::eRenderMessageType::eRenderPointLight:
			RenderPointLight(renderMessage);
			break;
		case SRenderMessage::eRenderMessageType::eRenderSpotLight:
			RenderSpotLight(renderMessage);
			break;
		}
	}
}

void CParticleRenderer::DoLight()
{
	SChangeStatesMessage changeStateMessage = {};

	myProcessed.Clear();
	myProcessed.Activate();
	SetSRV();
	SetCBuffer();

	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	mySharedRenderer.SetStates(&changeStateMessage);

	DoAmbientLighting();

	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eAddBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	mySharedRenderer.SetStates(&changeStateMessage);

	DoDirectLighting();
}

void CParticleRenderer::ToIntermediate()
{
	myInteremediate.Activate();
	mySharedHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredAmbient, &myProcessed);
}

void CParticleRenderer::ClearParticleTargets()
{
	myParticleGBuffer.diffuse.Clear();
	myParticleGBuffer.normal.Clear();
	myParticleGBuffer.RMAO.Clear();
	myParticleGBuffer.surfaceDiffuse.Clear();
	myParticleGBuffer.surfaceNormal.Clear();
	myParticleGBuffer.surfaceRMAO.Clear();
	myProcessed.Clear();
}

void CParticleRenderer::SetParticleTargets(ID3D11DepthStencilView* aDepthStencilView)
{
	ID3D11RenderTargetView* rtvs[3];
	rtvs[0] = myParticleGBuffer.diffuse.GetRenderTargetView();
	rtvs[1] = myParticleGBuffer.normal.GetRenderTargetView();
	rtvs[2] = myParticleGBuffer.RMAO.GetRenderTargetView();

	DEVICE_CONTEXT->OMSetRenderTargets(3, rtvs, aDepthStencilView);
}

void CParticleRenderer::SetSurfaceTargets(ID3D11DepthStencilView* aDepthStencilView)
{
	ID3D11RenderTargetView* rtvs[3];
	rtvs[0] = myParticleGBuffer.surfaceDiffuse.GetRenderTargetView();
	rtvs[1] = myParticleGBuffer.surfaceNormal.GetRenderTargetView();
	rtvs[2] = myParticleGBuffer.surfaceRMAO.GetRenderTargetView();

	DEVICE_CONTEXT->OMSetRenderTargets(3, rtvs, aDepthStencilView);
}


void CParticleRenderer::CreateSurface(ID3D11DepthStencilView* aDepthStencilView)
{
	SetSurfaceTargets(aDepthStencilView);
	SetSRV();
	
	mySharedHelper.DoEffect(CFullScreenHelper::eEffectType::eMetaSurface);
	SChangeStatesMessage message;
	message.myDepthStencilState = eDepthStencilState::eDefault;
	message.myBlendState = eBlendState::eAlphaBlend;
	message.myRasterizerState = eRasterizerState::eDefault;
	message.mySamplerState = eSamplerState::eClamp;
	mySharedRenderer.SetStates(&message);
	myParticleGBuffer.diffuse.Clear();
	myParticleGBuffer.diffuse.Activate();
	mySharedHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myParticleGBuffer.surfaceDiffuse);
	myParticleGBuffer.normal.Clear();
	myParticleGBuffer.normal.Activate();
	mySharedHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myParticleGBuffer.surfaceNormal);
	myParticleGBuffer.RMAO.Clear();
	myParticleGBuffer.RMAO.Activate();
	mySharedHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myParticleGBuffer.surfaceRMAO);

	message.myDepthStencilState = eDepthStencilState::eDisableDepth;
	message.myBlendState = eBlendState::eAlphaBlend;
	message.myRasterizerState = eRasterizerState::eDefault;
	message.mySamplerState = eSamplerState::eClamp;
	mySharedRenderer.SetStates(&message);
}
