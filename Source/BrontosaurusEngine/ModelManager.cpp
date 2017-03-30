#include "stdafx.h"
#include "Engine.h"
#include "ModelManager.h"
#include "ModelLoader.h"
#include "ConstBufferTemplate.h"
#include <CommonUtilities.h>
#include <CommandLineManager.h>
#include <matrix44.h>
#include <StringHelper.h>
#include <ThreadPool.h>

#include "GUIModleHelper.h"
#include "FBXLoader.h"
#include "..\GUI\GUIPixelConstantBuffer.h"


CModelManager::CModelManager()
	: myModelList(ourMaxNumberOfModels)
{
	myThreadPool = CEngine::GetInstance()->GetThreadPool();
}

CModelManager::~CModelManager()
{
}


const CModelManager::ModelId CModelManager::LoadModel(const std::string& aModelPath)
{
	if (myModelList.Size() >= ourMaxNumberOfModels)
	{
		DL_MESSAGE_BOX("Too many unique models created! Current max is: %i.\nTalk to a programmer if we need more, it is probably possible.\n", ourMaxNumberOfModels);
		DL_ASSERT("Too many unique models created! Current max is: %i.\n", ourMaxNumberOfModels);
		return NULL_MODEL;
	}

	ModelId newModelID = -1;

	if (myModels.count(aModelPath.c_str()) == 0) //check if derp, else flip bool
	{
		if (myFreeModelIDs.Size() != 0)
		{
			newModelID = myFreeModelIDs.Pop();
		}
		else
		{
			newModelID = myModelList.Size();
			myModelList.Add(CModel());
		}

		myModels[aModelPath.c_str()] = newModelID;

		if (CreateModel(aModelPath, newModelID) == false)
		{
			myModelList.Pop();
			myModels.erase(aModelPath.c_str());
			return NULL_MODEL;
		}

		LoadAnimations(aModelPath.c_str(), newModelID);
	}

	myModelList[myModels[aModelPath.c_str()]].AddRef();
	return myModels[aModelPath.c_str()];
}

const CModelManager::ModelId CModelManager::LoadGUIModel(const CLoaderMesh* aLoaderMesh, const char* aTexturePath)
{
	if (myModelList.Size() >= ourMaxNumberOfModels)
	{
		DL_MESSAGE_BOX("Too many unique models created! Current max is: %i.\nTalk to a programmer if you want more, it is probably possible.\n", ourMaxNumberOfModels);
		DL_ASSERT("Too many unique models created! Current max is: %i.\n", ourMaxNumberOfModels);
		return -1;
	}

	if (myModels.find(aLoaderMesh->myName) == myModels.end())
	{
		CEffect* effect = GUIModleHelper::CreateEffect(aLoaderMesh);
		CSurface* surface = GUIModleHelper::CreateSurface(aTexturePath);

		ModelId handle = -1;
		if (myFreeModelIDs.Size() != 0)
		{
			handle = myFreeModelIDs.Pop();
		}
		else
		{
			handle = myModelList.Size();
			myModelList.Add(CModel());
		}


		myModels[aLoaderMesh->myName] = handle;

		//myModelList[handle].Initialize(effect, surface, aLoaderMesh);

		SPixelConstantBuffer bufferStruct = {};
		//ID3D11Buffer* pixelConstantBuffer = BSR::CreateCBuffer<SPixelConstantBuffer>(&bufferStruct);
		//myModelList[handle].AddConstantBuffer(CModel::eShaderStage::ePixel, pixelConstantBuffer);
	}

	return myModels[aLoaderMesh->myName];
}
const CModelManager::ModelId CModelManager::LoadGUIModel(const CLoaderMesh * aLoaderMesh, const CU::GrowingArray<std::string>& aTexturePaths)
{
	if (myModelList.Size() >= ourMaxNumberOfModels)
	{
		DL_MESSAGE_BOX("Too many unique models created! Current max is: %i.\nTalk to a programmer if you want more, it is probably possible.\n", ourMaxNumberOfModels);
		DL_ASSERT("Too many unique models created! Current max is: %i.\n", ourMaxNumberOfModels);
		return -1;
	}

	if (myModels.find(aLoaderMesh->myName) == myModels.end())
	{
		CEffect* effect = GUIModleHelper::CreateEffect(aLoaderMesh);
		CSurface* surface = GUIModleHelper::CreateSurface(aTexturePaths);

		ModelId handle = -1;
		if (myFreeModelIDs.Size() != 0)
		{
			handle = myFreeModelIDs.Pop();
		}
		else
		{
			handle = myModelList.Size();
			myModelList.Add(CModel());
		}

		myModels[aLoaderMesh->myName] = handle;

		//myModelList[handle].Initialize(effect, surface, aLoaderMesh);

		SPixelConstantBuffer bufferStruct = {};
		//ID3D11Buffer* pixelConstantBuffer = BSR::CreateCBuffer<SPixelConstantBuffer>(&bufferStruct);
		//myModelList[handle].AddConstantBuffer(CModel::eShaderStage::ePixel, pixelConstantBuffer);
	}

	return myModels[aLoaderMesh->myName];
}

CModel* CModelManager::GetModel(const ModelId aModelID)
{
	if (aModelID == -1 || myModelList[aModelID].GetInitialized() == false)
	{
		return nullptr;
	}
	return &myModelList[aModelID];
}
void CModelManager::RemoveModel(const ModelId aModelID)
{
	myModelList[aModelID].RemoveRef();
	
	if (myModelList[aModelID].GetRefCount() <= 0)
	{
		myFreeModelIDs.Push(aModelID);
		myModelList[aModelID] = CModel();
	}
}

int CModelManager::GetModelRefCount(const ModelId aModelID) const
{
	if (aModelID == -1 || myModelList[aModelID].GetInitialized() == false)
	{
		return 0;
	}

	return myModelList[aModelID].GetRefCount();
}

void CModelManager::LoadAnimations(const char* aPath, const ModelId aModelId)
{
	static const std::string Directory("Models/Animations/");
	std::string modelName = aPath;
	DL_PRINT("modelName: %s", modelName.c_str());
	modelName -= std::string(".fbx");
	CU::FindAndReplace(modelName, "Meshes", "Animations");
	DL_PRINT("modelName - .fbx: %s", modelName.c_str());
	modelName.insert(Directory.length(), "ANI");
	DL_PRINT("modelName, insert ani: %s", modelName.c_str());
	DL_PRINT("modelName, replace meshes: %s", modelName.c_str());
	modelName += std::string("_");

	const ModelId animationCount = 3;
	const std::string animationNames[animationCount] = { ("idle01"), ("walk01"), ("shot01") };

	CModel* mdl = GetModel(aModelId);
	const aiScene* scene = mdl->GetScene();

	if (mdl != nullptr && /*scene->HasAnimations()*/mdl->HasBones())
	{
		mdl->myBindposeSceneAnimator = new CSceneAnimator();
		mdl->myBindposeSceneAnimator->Init(scene);

		mdl->mySceneAnimators.clear();
		CFBXLoader loader;
		for (int i = 0; i < animationCount; ++i)
		{
			const std::string& animationName = animationNames[i];

			const aiScene* animationScene = loader.GetScene(modelName + animationName + ".fbx");
			
			if (!animationScene)
			{
				continue;
			}

			mdl->mySceneAnimators[animationName] = CSceneAnimator();
			mdl->mySceneAnimators[animationName].Init(animationScene);
			//mdl->mySceneAnimators[animationName].CarlSave(modelName + animationName + ".animation");
			//mdl->mySceneAnimators[animationName].CarlLoad(modelName + animationName + ".animation");
		}

		if (mdl->mySceneAnimators.empty() == true)
		{
			mdl->mySceneAnimators["idle"] = CSceneAnimator();
			mdl->mySceneAnimators["idle"].Init(mdl->GetScene());
		}

		if (mdl->mySceneAnimators.find("idle") != mdl->mySceneAnimators.end())
		{
			mdl->mySceneAnimator = &mdl->mySceneAnimators["idle"];
		}
		else if (mdl->mySceneAnimators.begin() != mdl->mySceneAnimators.end())
		{
			mdl->mySceneAnimator = &mdl->mySceneAnimators.begin()->second;
		}
		else
		{
			mdl->mySceneAnimator = nullptr;
		}

		if (mdl->mySceneAnimator)
		{
			mdl->mySceneAnimator->PlayAnimationForward();
		}
	}
}

bool CModelManager::CreateModel(const std::string& aModelPath, ModelId aNewModel)
{
	return CModelLoader::LoadModel(aModelPath.c_str(), &myModelList[aNewModel]/*tempModelPointer*/);
}

bool CModelManager::DoesModelExists(const char * aModelPath)
{
	return myModels.find(aModelPath) != myModels.end();
}

