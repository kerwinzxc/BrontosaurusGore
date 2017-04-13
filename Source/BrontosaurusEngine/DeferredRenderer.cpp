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
#include "ParticleEmitterManager.h"
#include "ParticleEmitter.h"
#include "Texture.h"
#include "TextureManager.h"
#include "Engine.h"

#ifdef _ENABLE_RENDERMODES
#include <../CommonUtilities/InputWrapper.h>
#include "WindowsWindow.h"
#include "EKeyboardKeys.h"
#endif

CDeferredRenderer::CDeferredRenderer()
{
	myBatchedModelIds.Init(64);
	myFramework = CEngine::GetInstance()->GetFramework();
	CU::Vector2ui windowSize = CEngine::GetInstance()->GetWindowSize();

	//myGbuffer.Init(windowSize, myFramework, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);

	myIntermediatePackage.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	mySSAORandomTexture = &CEngine::GetInstance()->GetTextureManager().LoadTexture("SSAORandomTexture.dds");
	mySSAOPackage.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
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
	
	
#ifdef _ENABLE_RENDERMODES
	myInputWrapper = new CU::InputWrapper();
	myInputWrapper->Init(HINSTGET, HWNDGET);
	myRenderMode = ERenderMode::eIntermediate;
#endif
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

void CDeferredRenderer::DoRenderQueue(CRenderer& aRenderer)
{
	if (myGbuffer.IsInited() == false)
	{
		myRenderMessages.RemoveAll();
		return;
	}
	myGbuffer.Clear();	
	myGbuffer.UnbindInput();
	myGbuffer.BindOutput();

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


			if (msg->myRenderParams.myIgnoreDepth == true)
			{
				myGbuffer.BindOutput(CGeometryBuffer::eALL, CGeometryBuffer::eEmissive);
			}
			model->Render(msg->myRenderParams);
			if (msg->myRenderParams.myIgnoreDepth == true)
			{
				myGbuffer.BindOutput();
			}
			break;
		}
		case SRenderMessage::eRenderMessageType::eRenderModelInstanced:
		{
			SRenderModelInstancedMessage* msg = static_cast<SRenderModelInstancedMessage*>(myRenderMessages[i]);
			CModel* model = CEngine::GetInstance()->GetModelManager()->GetModel(msg->myModelID);
			model->AddInstanceToRenderBuffer(msg->myRenderParams);

			if (myBatchedModelIds.Find(msg->myModelID) == myBatchedModelIds.FoundNone)
			{
				myBatchedModelIds.Add(msg->myModelID);
			}
			break;
		}
		case SRenderMessage::eRenderMessageType::eRenderModelBatches:
		{
			SRenderModelBatches* msg = static_cast<SRenderModelBatches*>(myRenderMessages[i]);
			CModelManager* modelManager = CEngine::GetInstance()->GetModelManager();
			if (!modelManager) break;

			CModel* model = nullptr;

			for (int id : myBatchedModelIds)
			{
				model = modelManager->GetModel(id);
				if (!model) continue;

				model->RenderInstanced(msg->myRenderToDepth, msg->myPixelShader);
			}
			myBatchedModelIds.RemoveAll();
			break;
		}
		case SRenderMessage::eRenderMessageType::eChangeStates:
		{
			SChangeStatesMessage* msg = static_cast<SChangeStatesMessage*>(myRenderMessages[i]);
			aRenderer.SetStates(msg);
			break;
		}
		default:
			break;
		}
	}

	myRenderMessages.RemoveAll();
	myGbuffer.UnbindInput();
	myGbuffer.UnbindOutput();
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
	//case SRenderMessage::eRenderMessageType::eRenderModelDeferred:
	default:
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
	if (myGbuffer.IsInited() == false) return;
	SetCBuffer();

	//SSAO
#ifdef _ENABLE_RENDERMODES
	if(myRenderMode == ERenderMode::eSSAO || myRenderMode == ERenderMode::eIntermediate)
#endif
	{
		SChangeStatesMessage changeStateMessage = {};
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDefault;
		changeStateMessage.myBlendState = eBlendState::eMulBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoSSAO(aFullscreenHelper);
	}
	SChangeStatesMessage changeStateMessage = {};
	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDefault;
	changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	aRenderer.SetStates(&changeStateMessage);

	ActivateIntermediate();
	myGbuffer.BindInput();

#ifdef _ENABLE_RENDERMODES
	myInputWrapper->Update();
	HandleInput();

	switch (myRenderMode)
	{
	case ERenderMode::eDiffuse:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer.GetRenderPackage(CGeometryBuffer::eDiffuse));
		break;
	case ERenderMode::eNormal:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer.GetRenderPackage(CGeometryBuffer::eNormal));
		break;
	case ERenderMode::eRMAO:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer.GetRenderPackage(CGeometryBuffer::eRMAO));
		break;
	case ERenderMode::eRoughness:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyR, &myGbuffer.GetRenderPackage(CGeometryBuffer::eRMAO));
		break;
	case ERenderMode::eMetalness:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyG, &myGbuffer.GetRenderPackage(CGeometryBuffer::eRMAO));
		break;
	case ERenderMode::eAO:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyB, &myGbuffer.GetRenderPackage(CGeometryBuffer::eRMAO));
		break;
	case ERenderMode::eSSAO:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyB, &myGbuffer.GetRenderPackage(CGeometryBuffer::eRMAO));
		break;
	case ERenderMode::eEmissive:
	case ERenderMode::eHighlight:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer.GetRenderPackage(CGeometryBuffer::eEmissive));
		break;
	case ERenderMode::eIntermediate:
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage.myBlendState = eBlendState::eNoBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoAmbientLighting(aFullscreenHelper);
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage.myBlendState = eBlendState::eAddBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoDirectLighting(aFullscreenHelper);
		DoHighlight(aFullscreenHelper,aRenderer);
		break;
	default:
		break;
	}

#else
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage.myBlendState = eBlendState::eNoBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoAmbientLighting(aFullscreenHelper);
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage.myBlendState = eBlendState::eAddBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoDirectLighting(aFullscreenHelper);
		DoHighlight(aFullscreenHelper, aRenderer);
#endif
}

ID3D11DepthStencilView* CDeferredRenderer::GetDepthStencil()
{
	return myGbuffer.GetRenderPackage(CGeometryBuffer::eDiffuse).GetDepthStencilView();
}

ID3D11ShaderResourceView* CDeferredRenderer::GetDepthResource()
{
	return myGbuffer.GetRenderPackage(CGeometryBuffer::eDiffuse).GetDepthResource();
}

CRenderPackage& CDeferredRenderer::GetFirstPackage()
{
	return myGbuffer.GetRenderPackage(CGeometryBuffer::eDiffuse);
}

CRenderPackage& CDeferredRenderer::GetSecondPackage()
{
	return myGbuffer.GetRenderPackage(CGeometryBuffer::eEmissive);
}

void CDeferredRenderer::ActivateIntermediate()
{

	myGbuffer.UnbindOutput();
	myIntermediatePackage.Clear();
	myIntermediatePackage.Activate();
}

void CDeferredRenderer::SetRMAOSRV()
{
	myFramework->GetDeviceContext()->PSSetShaderResources(3, 1, &myGbuffer.GetRenderPackage(CGeometryBuffer::eRMAO).GetResource());
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

void CDeferredRenderer::DoHighlight(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer)
{
	SChangeStatesMessage changeStateMessage;
	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eEmilBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	aRenderer.SetStates(&changeStateMessage);
	//aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer.GetRenderPackage(CGeometryBuffer::eEmissive));
}

void CDeferredRenderer::DoSSAO(CFullScreenHelper& aFullscreenHelper)
{
	myGbuffer.UnbindOutput();
	myGbuffer.GetRenderPackage(CGeometryBuffer::eRMAO).Activate();
	CGeometryBuffer::EGeometryPackages packages = CGeometryBuffer::EGeometryPackage::eALL - CGeometryBuffer::EGeometryPackage::eRMAO;
	myGbuffer.BindInput(packages);

	myFramework->GetDeviceContext()->PSSetShaderResources(7, 1, mySSAORandomTexture->GetShaderResourceViewPointer());
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eSSAO);
}

void CDeferredRenderer::SetGeometryBuffer(CGeometryBuffer& aGeometryBuffer)
{
	myGbuffer = aGeometryBuffer;
}

#ifdef _ENABLE_RENDERMODES
void CDeferredRenderer::HandleInput()
{
	if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F1))
	{
		myRenderMode = ERenderMode::eIntermediate;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F1))
	{
		myRenderMode = ERenderMode::eDiffuse;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F2))
	{
		myRenderMode = ERenderMode::eNormal;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F3))
	{
		myRenderMode = ERenderMode::eRoughness;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F4))
	{
		myRenderMode = ERenderMode::eMetalness;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F5))
	{
		myRenderMode = ERenderMode::eAO;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F6))
	{
		myRenderMode = ERenderMode::eSSAO;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F7))
	{
		myRenderMode = (ERenderMode)((int)ERenderMode::eEmissive | (int)ERenderMode::eHighlight);
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F8))
	{
		myRenderMode = ERenderMode::eRMAO;
	}
}
#endif