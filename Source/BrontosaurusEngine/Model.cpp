#include "stdafx.h"
#include <assert.h>
#include "Model.h"
#include "Engine.h"
#include "DXFramework.h"
#include "Effect.h"
#include "BufferStructs.h"
#include "ConstBufferTemplate.h"
#include <Camera.h>
#include "Surface.h"
#include "CommonUtilities.h"
#include "PointLightInstance.h"
#include "FBXLoader.h"
#include "ModelInstance.h"
#include "Lights.h"
#include "../CommonUtilities/matrix44.h"
#include "../Collision/Intersection.h"
#include <CUTime.h>
#include <StringHelper.h>
#include "Renderer.h"

#include "../TShared/AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

CModel::CModel()
	: myForwardEffect(nullptr)
	, myDeferredEffect(nullptr)
	, mySurface(nullptr)
	, myFramework(nullptr)
	, myLODModels(4)
	, myCbuffer(nullptr)
	, myBoneBuffer(nullptr)
	, myLightBuffer(nullptr)
	, myPixelBuffer(nullptr)
	, myScene(nullptr)
	, myIsInitialized(false)
	, myIsAlphaModel(false)
	, mySceneAnimator(nullptr)
	, myBindposeSceneAnimator(nullptr)
	, myInstanceBuffer(nullptr)
	, myVertexSize(0)
	, myRefCount(0)
	, myInstanceBufferData(2)
{
}

CModel::CModel(const CModel& aCopy)
	: CModel()
{
	(*this) = aCopy;
}

CModel::~CModel()
{
	myFramework = nullptr;

	for (unsigned int i = 0; i < myLODModels.Size(); ++i)
	{
		SAFE_RELEASE(myLODModels[i].myIndexBuffer);
		SAFE_RELEASE(myLODModels[i].myVertexBuffer);
	}

	SAFE_RELEASE(myCbuffer);
	SAFE_RELEASE(myLightBuffer);
	SAFE_RELEASE(myPixelBuffer);
	SAFE_RELEASE(myInstanceBuffer);
	myIsInitialized = false;
	myVertexSize = 0;

	SAFE_DELETE(myDeferredEffect); //make sure this releases stuff
	SAFE_DELETE(myForwardEffect); //make sure this releases stuff

	SAFE_DELETE(mySurface);

	mySceneAnimator = nullptr;
	SAFE_DELETE(myBindposeSceneAnimator);
}

bool CModel::Initialize(CEffect* aEffect, CSurface* aSurface)
{
	mySurface = aSurface;
	myFramework = CEngine::GetInstance()->GetFramework();
	myForwardEffect = aEffect;
	myIsInitialized = true;
	InitConstBuffers();
	return true;
}
bool CModel::Initialize(CEffect* aEffect, CSurface* aSurface, const CU::GrowingArray<CLoaderMesh*>& aLoadedMeshList)
{
#ifdef _DEBUG
	myFilePath = aLoadedMeshList.GetFirst()->myName;
#endif // _DEBUG

	Initialize(aEffect, aSurface);
	myRadius = (aLoadedMeshList.GetLast()->myMaxPoint - aLoadedMeshList.GetLast()->myMinPoint).Length();

	myIsAlphaModel = aLoadedMeshList.GetLast()->myIsAlphaMesh;
	myVertexSize = aLoadedMeshList.GetLast()->myVertexBufferSize;

	for (unsigned int i = 0; i < aLoadedMeshList.Size(); ++i)
	{
		myLODModels.Add(SLodData());
		myLODModels.GetLast().myLodLevel = aLoadedMeshList[i]->myLODLevel;



		float temp = SQUARE(aLoadedMeshList[i]->myLOD_DistStart);
		myLODModels.GetLast().myDistances.LOD_DistStart2 = temp * 10;

		temp = SQUARE(aLoadedMeshList[i]->myLOD_DistEnd);
		myLODModels.GetLast().myDistances.LOD_DistEnd2 = temp * 10;

		if (InitBuffers(aLoadedMeshList[i]) == false)
		{
			DL_ASSERT("Failed To create LOD:", i, " from path: ", aLoadedMeshList[i]->myName);
		}
	}

	return true;
}

bool CModel::InitBuffers(const CLoaderMesh* aLoadedMesh)
{
	myLODModels.GetLast().myIndexCount = static_cast<unsigned int>(aLoadedMesh->myIndexes.size());
	myLODModels.GetLast().myVertexCount = static_cast<unsigned int>(aLoadedMesh->myVertexCount);

	// VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = aLoadedMesh->myVertexCount * myVertexSize;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = aLoadedMesh->myVerticies;
	HRESULT result;
	result = DEVICE->CreateBuffer(&vertexBufferDesc, &vertexData, &myLODModels.GetLast().myVertexBuffer);
	CHECK_RESULT(result, "Failed to create vertexbuffer.");

	// INDEX BUFFER
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(int) * myLODModels.GetLast().myIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = &aLoadedMesh->myIndexes[0];
	result = DEVICE->CreateBuffer(&indexBufferDesc, &indexData, &myLODModels.GetLast().myIndexBuffer);
	CHECK_RESULT(result, "Failed to create indexbuffer.");

	///////////////////////////////////////////////////////
	//				INSTANCE BUFFER						 //
	///////////////////////////////////////////////////////

	D3D11_BUFFER_DESC instanceBufferDesc;
	ZeroMemory(&instanceBufferDesc, sizeof(instanceBufferDesc));

	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = ourMaxInstanceSize;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;


	result = DEVICE->CreateBuffer(&instanceBufferDesc, nullptr, &myInstanceBuffer);
	CHECK_RESULT(result, "Failed to create instanceBuffer.");
	///////////////////////////////////////////////////////

	
	//ANIMATION BUFFER
	if (aLoadedMesh->myScene != nullptr && /*aLoadedMesh->myScene->myScene->mNumAnimations*/(aLoadedMesh->myShaderType & EModelBluePrint_Bones) > 0)
	{
		SAnimationBoneStruct boneBuffer;
		myBoneBuffer = BSR::CreateCBuffer<SAnimationBoneStruct>(&boneBuffer);
	}
	return true;
}

void CModel::Render(SForwardRenderModelParams& aParamObj)
{
	if (aParamObj.myRenderToDepth == true)
	{
		myForwardEffect->ActivateForDepth();
	}
	else
	{
		myForwardEffect->Activate();
	}

	if (mySurface != nullptr)
	{
		mySurface->Activate();
	}
	
	UpdateCBuffer(aParamObj);

	UINT stride = myVertexSize;
	UINT offset = 0;

	SLodData& currentLodModel = GetCurrentLODModel(aParamObj.myTransform.GetPosition());

	DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &currentLodModel.myVertexBuffer, &stride, &offset);
	DEVICE_CONTEXT->IASetIndexBuffer(currentLodModel.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	if (myLODModels.GetLast().myIndexBuffer == nullptr)
	{
		DEVICE_CONTEXT->Draw(currentLodModel.myVertexCount, 0);
	}
	else
	{
		DEVICE_CONTEXT->DrawIndexed(currentLodModel.myIndexCount, 0, 0);
	}
}

void CModel::Render(SDeferredRenderModelParams& aParamObj)
{
	if (aParamObj.myRenderToDepth == true)
	{
		myDeferredEffect->ActivateForDepth();
	}
	else
	{
		myDeferredEffect->Activate();
	}

	if (mySurface != nullptr)
	{
		mySurface->Activate();
	}

	UpdateCBuffer(aParamObj);

	UINT stride = myVertexSize;
	UINT offset = 0;

	SLodData& currentLodModel = GetCurrentLODModel(aParamObj.myTransform.GetPosition());

	DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &currentLodModel.myVertexBuffer, &stride, &offset);
	DEVICE_CONTEXT->IASetIndexBuffer(currentLodModel.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DEVICE_CONTEXT->DrawIndexed(currentLodModel.myIndexCount, 0, 0);
	
}

//
//void CModel::Render(SShadowRenderModelParams& aParamObj)
//{
//	myForwardEffect->ActivateForDepth(aParamObj.aPixelshader);
//
//	if (mySurface != nullptr)
//	{
//		mySurface->Activate();
//	}
//
//	UpdateCBuffer(aParamObj);
//
//	UINT stride = myVertexSize;
//	UINT offset = 0;
//
//	SLodData& currentLodModel = GetCurrentLODModel(aParamObj.myTransform.GetPosition());
//
//	DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &currentLodModel.myVertexBuffer, &stride, &offset);
//	DEVICE_CONTEXT->IASetIndexBuffer(currentLodModel.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	if (myLODModels.GetLast().myIndexBuffer == nullptr)
//	{
//		DEVICE_CONTEXT->Draw(currentLodModel.myVertexCount, 0);
//	}
//	else
//	{
//		DEVICE_CONTEXT->DrawIndexed(currentLodModel.myIndexCount, 0, 0);
//	}
//}

void CModel::RenderInstanced(const bool aRenderDepth, ID3D11PixelShader* aDepthShader)
{
	if (aRenderDepth)
	{
		myDeferredEffect->ActivateForDepth(aDepthShader, true);
	}
	else
	{
		myDeferredEffect->Activate(true);
		if (mySurface != nullptr)
		{
			mySurface->Activate();
		}
	}

	ID3D11DeviceContext* context = DEVICE_CONTEXT;

	context->IASetIndexBuffer(myLODModels.GetLast().myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	const unsigned int drawCalls = myInstanceBufferData.Size() / ourMaxInstances + 1u;
	for (unsigned int i = 0; i < drawCalls; ++i)
	{
		UpdateInstanceBuffer(i * (ourMaxInstances - 1));
		unsigned int strides[2] = { myVertexSize, sizeof(SToWorldSpace) };
		unsigned int offsets[2] = { 0, 0 };

		ID3D11Buffer* buffers[2] = { myLODModels.GetLast().myVertexBuffer, myInstanceBuffer };

		context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
		unsigned int instanceCount = myInstanceBufferData.Size() - i * ourMaxInstances;
		instanceCount = min(instanceCount, ourMaxInstances);
		context->DrawIndexedInstanced(myLODModels.GetLast().myIndexCount, instanceCount, 0, 0, 0);
	}

	myInstanceBufferData.RemoveAll();
}

void CModel::AddInstanceToRenderBuffer(const SDeferredRenderModelParams& aParamObj)
{
	SToWorldSpace instanceData;
	instanceData.myHighlightColor = aParamObj.myHighlightColor;
	instanceData.myHighlightColor.a = aParamObj.aHighlightIntencity;
	instanceData.myWorldSpace = aParamObj.myTransform;
	instanceData.myWorldSpaceLastFrame = aParamObj.myTransformLastFrame;
	myInstanceBufferData.Add(instanceData);
}

void CModel::UpdateCBuffer(SForwardRenderModelParams& aParamObj)
{
	assert(false && "Carl was here :)");
	// WorldSpace thingy
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	ZeroMemory(&mappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	SToWorldSpace updated;
	updated.myWorldSpaceLastFrame = aParamObj.myTransform;
	updated.myWorldSpace = aParamObj.myTransformLastFrame;
	DEVICE_CONTEXT->Map(myCbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	memcpy(mappedSubResource.pData, &updated, sizeof(SToWorldSpace));
	DEVICE_CONTEXT->Unmap(myCbuffer, 0);
	DEVICE_CONTEXT->VSSetConstantBuffers(1, 1, &myCbuffer);

	//ANIMATION BUFFER
	if (mySceneAnimator != nullptr && (aParamObj.aAnimationState != eAnimationState::none))
	{
		std::vector<mat4>& bones = GetBones(aParamObj.aAnimationTime, aParamObj.aAnimationState, aParamObj.aAnimationLooping);

		ZeroMemory(&mappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		DEVICE_CONTEXT->Map(myBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
		memcpy(mappedSubResource.pData, &bones[0], min(ourMaxBoneBufferSize, bones.size() * sizeof(mat4)));
		DEVICE_CONTEXT->Unmap(myBoneBuffer, 0);
		DEVICE_CONTEXT->VSSetConstantBuffers(3, 1, &myBoneBuffer);
	}

	if (aParamObj.myRenderToDepth == false)
	{
		//LIGHT

		ZeroMemory(&mappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		Lights::SLightsBuffer updatedLights;
		updatedLights.myDirectionalLight = aParamObj.myDirectionalLight;

		for (unsigned int i = 0; i < aParamObj.myPointLightList.Size(); ++i)
		{
			updatedLights.myPointLights[i] = aParamObj.myPointLightList[i];
		}

		updatedLights.cubeMap_mipCount = 11; // TODO FIX WTH?!
		updatedLights.highlightColor = { 1.f, 0.f, 0.f, aParamObj.aHighlightIntencity };
		updatedLights.myNumberOfLights = aParamObj.myNumLights;

		DEVICE_CONTEXT->Map(myLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
		memcpy(mappedSubResource.pData, &updatedLights, sizeof(Lights::SLightsBuffer));
		DEVICE_CONTEXT->Unmap(myLightBuffer, 0);
		DEVICE_CONTEXT->PSSetConstantBuffers(1, 1, &myLightBuffer);
	}
}
void CModel::UpdateCBuffer(SDeferredRenderModelParams& aParamObj)
{
	// WorldSpace thingy
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	ZeroMemory(&mappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	SToWorldSpace updated;
	updated.myWorldSpace = aParamObj.myTransform;
	updated.myWorldSpaceLastFrame = aParamObj.myTransformLastFrame;
	updated.myHighlightColor = aParamObj.myHighlightColor;
	DEVICE_CONTEXT->Map(myCbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	memcpy(mappedSubResource.pData, &updated, sizeof(SToWorldSpace));
	DEVICE_CONTEXT->Unmap(myCbuffer, 0);
	DEVICE_CONTEXT->VSSetConstantBuffers(1, 1, &myCbuffer);
	DEVICE_CONTEXT->PSSetConstantBuffers(1, 1, &myCbuffer);
#ifdef _DEBUG
	if (myFilePath.find("lasma") != std::string::npos)
	{
		int br = 0;
	}
#endif

	//ANIMATION BUFFER
	if (mySceneAnimator != nullptr && (aParamObj.aAnimationState != eAnimationState::none) /*&& (aParamObj.aAnimationState[0] != '\0')*/)
	{
		const std::vector<mat4>& bones = GetBones(aParamObj.aAnimationTime, aParamObj.aAnimationState, aParamObj.aAnimationLooping);
		const std::vector<mat4>* finalBones = &bones;

		std::vector<mat4> blendedBones;

		if (aParamObj.aNextAnimationState != eAnimationState::none && aParamObj.aAnimationState != aParamObj.aNextAnimationState)
		{
			const std::vector<mat4>& nextBones = GetBones(aParamObj.aAnimationTime, aParamObj.aNextAnimationState, aParamObj.aAnimationLooping);

			blendedBones.resize(bones.size());
			BlendBones(bones, nextBones, aParamObj.aAnimationLerper, blendedBones);
			finalBones = &blendedBones;
		}
		unsigned int bytesToCopy = min(ourMaxBoneBufferSize, finalBones->size() * sizeof(mat4));
		if (bytesToCopy > 0)
		{
			ZeroMemory(&mappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			DEVICE_CONTEXT->Map(myBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
			memcpy(mappedSubResource.pData, finalBones->data(), bytesToCopy);
			DEVICE_CONTEXT->Unmap(myBoneBuffer, 0);
			DEVICE_CONTEXT->VSSetConstantBuffers(3, 1, &myBoneBuffer);
		}
	}
}
//void CModel::UpdateCBuffer(SShadowRenderModelParams& aParamObj)
//{
//	SDeferredRenderModelParams params;
//	params.aAnimationLooping = aParamObj.aAnimationLooping;
//	params.aAnimationState = aParamObj.aAnimationState;
//	params.aNextAnimationState = aParamObj.aNextAnimationState;
//	params.aAnimationLerper = aParamObj.aAnimationLerper;
//	params.aAnimationTime = aParamObj.aAnimationTime;
//	params.aHighlightIntencity = aParamObj.aHighlightIntencity;
//	params.myTransform = aParamObj.myTransform;
//	params.myTransformLastFrame = aParamObj.myTransformLastFrame;
//	params.myRenderToDepth = false;
//	UpdateCBuffer(params);
//}

void CModel::UpdateInstanceBuffer(const unsigned int aStartIndex)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	ZeroMemory(&mappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HRESULT hr = DEVICE_CONTEXT->Map(myInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	CHECK_RESULT(hr, "Failed to map instancebuffer");
	size_t byteSize = min(myInstanceBufferData.ByteSize(), ourMaxInstanceSize);
	memcpy(mappedSubResource.pData, myInstanceBufferData.AsVoidPointer(aStartIndex), byteSize);
	DEVICE_CONTEXT->Unmap(myInstanceBuffer, 0);
}

#include "../Physics/PhysXHelper.h"

void CModel::BlendBones(const std::vector<mat4>& aBlendFrom, const std::vector<mat4>& aBlendTo, const float aLerpValue, std::vector<mat4>& aBlendOut)
{
	for (size_t i = 0; i < aBlendFrom.size(); ++i)
	{
		CU::Matrix44f lerpedid = aBlendFrom[i].Lerp(aBlendTo[i].GetRotation(), aLerpValue);
		CU::Matrix44f slerped = Physics::Slerp(aBlendFrom[i].GetRotation(), aBlendTo[i].GetRotation(), aLerpValue);
		CU::Matrix44f difference = lerpedid - slerped;
		aBlendOut[i] = Physics::Slerp(aBlendFrom[i].GetRotation(), aBlendTo[i].GetRotation(), aLerpValue);
		//aBlendOut[i] = aBlendFrom[i].Lerp(aBlendTo[i], aLerpValue);
		//aBlendOut[i] = aBlendFrom[i].SlerpRotation(aBlendTo[i], aLerpValue);
		aBlendOut[i].myPosition = aBlendFrom[i].myPosition.Lerp(aBlendTo[i].myPosition, aLerpValue);
	}
}

std::vector<mat4>& CModel::GetBones(float aTime, const eAnimationState aAnimationState, const bool aAnimationLooping)
{
	if(mySceneAnimator != nullptr)
	{
		auto it = mySceneAnimators.find(aAnimationState);
		if (it != mySceneAnimators.end())
		{
			mySceneAnimator = &it->second;
		}

		return mySceneAnimator->GetTransforms(aTime, aAnimationLooping);
	}

	static std::vector<mat4> locNullBones = { mat4::Identity };

	return locNullBones;
}

SLodData& CModel::GetCurrentLODModel(const CU::Vector3f& /*aModelPosition*/)
{
	if (myLODModels.Size() > 1)
	{
		//TODO: FIX CAMERA :)
		float distanceToCam2 = 0;// Intersection::Distance2Points2(CAMERA->GetPosition(), aModelPosition);
		for (unsigned int i = 0; i < myLODModels.Size(); ++i)
		{
			if (distanceToCam2 >= myLODModels[i].myDistances.LOD_DistStart2 && distanceToCam2 <= myLODModels[i].myDistances.LOD_DistEnd2)
			{
				return myLODModels[i];
			}
		}
	}

	return myLODModels.GetLast();
}

void CModel::InitConstBuffers()
{
	SToWorldSpace bufferData;
	myCbuffer = BSR::CreateCBuffer<SToWorldSpace>(&bufferData);

	Lights::SLightsBuffer light;
	myLightBuffer = BSR::CreateCBuffer<Lights::SLightsBuffer>(&light);

	
}

CU::Matrix44f CModel::GetBoneTransform(const float aTime, const eAnimationState aAnimationState, const char* aBoneName)
{
	if (mySceneAnimator != nullptr)
	{
		if (mySceneAnimator != nullptr)
		{
			auto it = mySceneAnimators.find(aAnimationState);
			if (it != mySceneAnimators.end())
			{
				mySceneAnimator = &it->second;
			}
		}

		int bindex = myBindposeSceneAnimator->GetBoneIndex(aBoneName);
		if (bindex == -1)
			return mat4();

		CU::Matrix44f retVal = myBindposeSceneAnimator->GetBoneGlobalTransform(bindex);
		retVal *= mySceneAnimator->GetBoneTransform(aTime, bindex);


		return retVal;
	}
	return CU::Matrix44f::Identity;
}

CModel& CModel::operator=(CModel&& aModel)
{
	SAFE_DELETE(myForwardEffect);
	myForwardEffect = aModel.myForwardEffect;
	aModel.myForwardEffect = nullptr;

	SAFE_DELETE(myDeferredEffect);
	myDeferredEffect = aModel.myDeferredEffect;
	aModel.myDeferredEffect = nullptr;


	SAFE_DELETE(mySurface);
	mySurface = aModel.mySurface;
	aModel.mySurface = nullptr;

	myFramework = aModel.myFramework;
	aModel.myFramework = nullptr;

	for (unsigned int i = 0; i < myLODModels.Size(); ++i)
	{
		SLodData& lodModel = myLODModels[i];

		SAFE_RELEASE(lodModel.myVertexBuffer);
		SAFE_RELEASE(myLODModels[i].myIndexBuffer);
	}

	myLODModels.RemoveAll();

	for (unsigned int i = 0; i < aModel.myLODModels.Size(); ++i)
	{
		SLodData& otherLodModel = aModel.myLODModels[i];
		
		myLODModels.Add(SLodData());
		SLodData& lodModel = myLODModels.GetLast();

		lodModel.myDistances = otherLodModel.myDistances;

		lodModel.myVertexBuffer = otherLodModel.myVertexBuffer;
		otherLodModel.myVertexBuffer = nullptr;
		myLODModels[i].myIndexBuffer = otherLodModel.myIndexBuffer;
		otherLodModel.myIndexBuffer = nullptr;

		lodModel.myLodLevel = otherLodModel.myLodLevel;
		lodModel.myVertexCount = otherLodModel.myVertexCount;
		lodModel.myIndexCount = otherLodModel.myIndexCount;
	}

	myCbuffer = aModel.myCbuffer;
	aModel.myCbuffer = nullptr;
	myLightBuffer = aModel.myLightBuffer;
	aModel.myLightBuffer = nullptr;


	myInstanceBuffer = aModel.myInstanceBuffer;
	aModel.myInstanceBuffer = nullptr;

	myIsInitialized = aModel.myIsInitialized.load();
	aModel.myIsInitialized = false;

	myVertexSize = aModel.myVertexSize;
	aModel.myVertexSize = 0;

	myScene = aModel.myScene;
	aModel.myScene = nullptr;

	return *this;
}
CModel& CModel::operator=(const CModel& aModel)
{

	SAFE_DELETE(myForwardEffect);
	if (aModel.myForwardEffect != nullptr)
	{
		myForwardEffect = new CEffect(*aModel.myForwardEffect);
	}
	SAFE_DELETE(myDeferredEffect);
	if (aModel.myDeferredEffect != nullptr)
	{
		myDeferredEffect = new CEffect(*aModel.myDeferredEffect);
	}


	SAFE_DELETE(mySurface);
	if (aModel.mySurface != nullptr)
	{
		mySurface = new CSurface(*aModel.mySurface);
	}

	myFramework = aModel.myFramework;

	for (unsigned int i = 0; i < myLODModels.Size(); ++i)
	{
		SLodData& lodModel = myLODModels[i];

		SAFE_RELEASE(lodModel.myVertexBuffer);
		SAFE_RELEASE(myLODModels[i].myIndexBuffer);
	}

	myLODModels.RemoveAll();

	for (unsigned int i = 0; i < aModel.myLODModels.Size(); ++i)
	{
		const SLodData& otherLodModel = aModel.myLODModels[i];

		myLODModels.Add(SLodData());
		SLodData& lodModel = myLODModels.GetLast();

		lodModel.myDistances = otherLodModel.myDistances;

		lodModel.myVertexBuffer = otherLodModel.myVertexBuffer;
		SAFE_ADD_REF(lodModel.myVertexBuffer);
		lodModel.myIndexBuffer = otherLodModel.myIndexBuffer;
		SAFE_ADD_REF(lodModel.myIndexBuffer);

		lodModel.myLodLevel = otherLodModel.myLodLevel;
		lodModel.myVertexCount = otherLodModel.myVertexCount;
		lodModel.myIndexCount = otherLodModel.myIndexCount;
	}

	myCbuffer = aModel.myCbuffer;
	SAFE_ADD_REF(myCbuffer);
	myLightBuffer = aModel.myLightBuffer;
	SAFE_ADD_REF(myLightBuffer);

	myInstanceBuffer = aModel.myInstanceBuffer;
	SAFE_ADD_REF(myInstanceBuffer);


	myIsInitialized = aModel.myIsInitialized.load();

	myVertexSize = aModel.myVertexSize;

	myScene = aModel.myScene;

	return *this;
}
