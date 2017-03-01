#include "stdafx.h"
#include "DeferredRenderer.h"
#include "RenderMessages.h"
#include "Engine.h"
#include "DXFramework.h"
#include "ModelManager.h"
#include "ConstBufferTemplate.h"
#include "FullScreenHelper.h"
#include "Lights.h"
#include "Renderer.h"
#include "LightModel.h"
#include "Engine.h"
#include "ShaderManager.h"
#include "FBXLoader.h"

CDeferredRenderer::CDeferredRenderer()
{
	myFramework = CEngine::GetInstance()->GetFramework();
	CU::Vector2ui windowSize = CEngine::GetInstance()->GetWindowSize();

	myGbuffer.diffuse.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myGbuffer.normal.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myGbuffer.emissive.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myGbuffer.RMAO.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);

	myIntermediatePackage.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	
	

	myRenderMessages.Init(128);
	myLightMessages.Init(128);

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

CDeferredRenderer::~CDeferredRenderer()
{
	SAFE_RELEASE(myProjectionInverseBuffer);
	SAFE_RELEASE(myDirectionalLightBuffer);
	SAFE_RELEASE(myPointLightBuffer);
	SAFE_RELEASE(mySpotLightBuffer);
	SAFE_DELETE(myLightModel);
}

void CDeferredRenderer::InitPointLightModel()
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

void CDeferredRenderer::DoRenderMessage(SRenderMessage* aRenderMessage)
{
	switch (aRenderMessage->myType)
	{
	case SRenderMessage::eRenderMessageType::eRenderModelDeferred:
	{
		SRenderModelDeferredMessage* msg = static_cast<SRenderModelDeferredMessage*>(aRenderMessage);
		CModelManager* modelManager = CEngine::GetInstance()->GetModelManager();
		if (!modelManager) break;
		CModel* model = modelManager->GetModel(msg->myModelID);
		if (!model) break;
		model->Render(msg->myRenderParams);
		break;
	}
	default:
		break;
	}
}

void CDeferredRenderer::DoRenderQueue()
{
	ClearRenderTargets();
	SetRenderTargets();

	for (unsigned int i = 0; i < myRenderMessages.Size(); ++i)
	{
		switch (myRenderMessages[i]->myType)
		{
		case SRenderMessage::eRenderMessageType::eRenderModelDeferred:
		{
			SRenderModelDeferredMessage* msg = static_cast<SRenderModelDeferredMessage*>(myRenderMessages[i]);
			CModelManager* modelManager = CEngine::GetInstance()->GetModelManager();
			if (!modelManager) break;

			CModel* model = modelManager->GetModel(msg->myModelID);
			if (!model) break;

			model->Render(msg->myRenderParams);
			break;
		}
		default:
			break;
		}
	}
	myRenderMessages.RemoveAll();
}

void CDeferredRenderer::AddRenderMessage(SRenderMessage* aRenderMessage)
{
	switch (aRenderMessage->myType)
	{
	case SRenderMessage::eRenderMessageType::eRenderSpotLight:
	case SRenderMessage::eRenderMessageType::eRenderPointLight:
	case SRenderMessage::eRenderMessageType::eRenderDirectionalLight:
		myLightMessages.Add(aRenderMessage);
		break;
	case SRenderMessage::eRenderMessageType::eRenderModelDeferred:
		myRenderMessages.Add(aRenderMessage);
		break;
	}
}

void CDeferredRenderer::UpdateCameraBuffer(const CU::Matrix44f & aCameraSpace, const CU::Matrix44f & aProjectionInverse)
{
	SCameraStruct updatedMatrices;
	updatedMatrices.prjInv = aProjectionInverse;
	updatedMatrices.camTW = aCameraSpace;

	BSR::UpdateCBuffer<SCameraStruct>(myProjectionInverseBuffer, &updatedMatrices);
}

void CDeferredRenderer::DoLightingPass(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer)
{
	ActivateIntermediate();
	SetSRV();
	SetCBuffer();

	SChangeStatesMessage changeStateMessage = {};
	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eNoBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	aRenderer.SetStates(&changeStateMessage);

	DoAmbientLighting(aFullscreenHelper);

	changeStateMessage.myRasterizerState = eRasterizerState::eCullFront;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eAddBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	aRenderer.SetStates(&changeStateMessage);

	DoDirectLighting(aFullscreenHelper);
}

void CDeferredRenderer::SetRenderTargets()
{
	ID3D11ShaderResourceView* srvs[5];
	srvs[0] = nullptr;
	srvs[1] = nullptr;
	srvs[2] = nullptr;
	srvs[3] = nullptr;
	srvs[4] = nullptr;

	myFramework->GetDeviceContext()->PSSetShaderResources(1, 5, srvs);


	ID3D11RenderTargetView* rtvs[4];
	rtvs[0] = myGbuffer.diffuse.GetRenderTargetView();
	rtvs[1] = myGbuffer.normal.GetRenderTargetView();
	rtvs[2] = myGbuffer.RMAO.GetRenderTargetView();
	rtvs[3] = myGbuffer.emissive.GetRenderTargetView();

	myFramework->GetDeviceContext()->OMSetRenderTargets(4, rtvs, myGbuffer.diffuse.GetDepthStencilView());
}

void CDeferredRenderer::ClearRenderTargets()
{
	myGbuffer.diffuse.Clear();
	myGbuffer.normal.Clear();
	myGbuffer.RMAO.Clear();
	myGbuffer.emissive.Clear();
}

void CDeferredRenderer::ActivateIntermediate()
{
	ID3D11RenderTargetView* rtvs[4];
	rtvs[0] = nullptr;
	rtvs[1] = nullptr;
	rtvs[2] = nullptr;
	rtvs[3] = nullptr;
	myFramework->GetDeviceContext()->OMSetRenderTargets(4, rtvs, nullptr);


	myIntermediatePackage.Clear();
	myIntermediatePackage.Activate();
}

void CDeferredRenderer::SetSRV()
{
	ID3D11ShaderResourceView* srvs[5];
	srvs[0] = myGbuffer.diffuse.GetResource();
	srvs[1] = myGbuffer.normal.GetResource();
	srvs[2] = myGbuffer.RMAO.GetResource();
	srvs[3] = myGbuffer.emissive.GetResource();
	srvs[4] = myGbuffer.diffuse.GetDepthResource();
	myFramework->GetDeviceContext()->PSSetShaderResources(1, 5, srvs);
}

void CDeferredRenderer::SetCBuffer()
{
	myFramework->GetDeviceContext()->PSSetConstantBuffers(1, 1, &myProjectionInverseBuffer);
}

void CDeferredRenderer::DoAmbientLighting(CFullScreenHelper& aFullscreenHelper)
{
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredAmbient);
}

void CDeferredRenderer::DoDirectLighting(CFullScreenHelper& aFullscreenHelper)
{
	for (SRenderMessage* renderMessage : myLightMessages)
	{
		switch (renderMessage->myType)
		{
		case SRenderMessage::eRenderMessageType::eRenderDirectionalLight:
			RenderDirectionalLight(renderMessage, aFullscreenHelper);
			break;
		case SRenderMessage::eRenderMessageType::eRenderPointLight:
			RenderPointLight(renderMessage, aFullscreenHelper);
			break;
		case SRenderMessage::eRenderMessageType::eRenderSpotLight:
			RenderSpotLight(renderMessage, aFullscreenHelper);
			break;
		}
	}
	myLightMessages.RemoveAll();
}

void CDeferredRenderer::RenderDirectionalLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper)
{
	SRenderDirectionalLight* msg = static_cast<SRenderDirectionalLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SDirectionalLight>(myDirectionalLightBuffer, &msg->directionalLight);
	myFramework->GetDeviceContext()->PSSetConstantBuffers(2, 1, &myDirectionalLightBuffer);
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredDirectional);
}

void CDeferredRenderer::RenderPointLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper)
{
	SRenderPointLight* msg = static_cast<SRenderPointLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SPointLight>(myPointLightBuffer, &msg->pointLight);
	myFramework->GetDeviceContext()->PSSetConstantBuffers(2, 1, &myPointLightBuffer);
	CU::Matrix44f pointLightTransformation;
	pointLightTransformation.myPosition = msg->pointLight.position;
	float scale = msg->pointLight.range;
	pointLightTransformation.Scale({ scale, scale, scale });
	myLightModel->Render(pointLightTransformation);
}

void CDeferredRenderer::RenderSpotLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper)
{
	SRenderSpotLight* msg = static_cast<SRenderSpotLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SSpotLight>(mySpotLightBuffer, &msg->spotLight);
	myFramework->GetDeviceContext()->PSSetConstantBuffers(2, 1, &mySpotLightBuffer);
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredSpotLight);
}

