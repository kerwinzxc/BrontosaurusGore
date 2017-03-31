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


#include "../TShared/AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

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
	modelName -= std::string(".fbx");
	CU::FindAndReplace(modelName, "Meshes", "Animations");
	modelName.insert(Directory.length(), "ANI");
	modelName += std::string("_");

	CModel* mdl = GetModel(aModelId);
	const aiScene* scene = mdl->GetScene();

	if (mdl != nullptr && /*scene->HasAnimations()*/mdl->HasBones())
	{
		mdl->myBindposeSceneAnimator = new CSceneAnimator();
		mdl->myBindposeSceneAnimator->Init(scene);

		mdl->mySceneAnimators.clear();
		CFBXLoader loader;
		for (int i = 0; i < SAnimationState::AnimationStates.Size(); ++i)
		{
			const std::string& animationName = SAnimationState::AnimationStates[i];

			const aiScene* animationScene = loader.GetScene(modelName + animationName + ".fbx");
			
			if (!animationScene)
			{
				continue;
			}

			mdl->mySceneAnimators[static_cast<eAnimationState>(i)] = CSceneAnimator();
			mdl->mySceneAnimators[static_cast<eAnimationState>(i)].Init(animationScene);

			//mdl->mySceneAnimators[animationName] = CSceneAnimator();
			//mdl->mySceneAnimators[animationName].Init(animationScene);

			//mdl->mySceneAnimators[animationName].CarlSave(modelName + animationName + ".animation");
			//mdl->mySceneAnimators[animationName].CarlLoad(modelName + animationName + ".animation");
		}

		if (mdl->mySceneAnimators.empty() == true)
		{
			mdl->mySceneAnimators[eAnimationState::idle01] = CSceneAnimator();
			mdl->mySceneAnimators[eAnimationState::idle01].Init(mdl->GetScene());
			//mdl->mySceneAnimators["idle"] = CSceneAnimator();
			//mdl->mySceneAnimators["idle"].Init(mdl->GetScene());
		}

		if (mdl->mySceneAnimators.find(/*"idle"*/eAnimationState::idle01) != mdl->mySceneAnimators.end())
		{
			mdl->mySceneAnimator = &mdl->mySceneAnimators[/*"idle"*/eAnimationState::idle01];
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

