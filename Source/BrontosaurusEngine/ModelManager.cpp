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
	: myModelList(64)
{
}

CModelManager::~CModelManager()
{
}

const CModelManager::ModelId CModelManager::LoadModel(const std::string& aModelPath)
{
	ModelId newModelID = -1;

	if (myModels.find(aModelPath) == myModels.end()) //check if derp, else flip bool
	{
		if (myFreeModelIDs.Size() != 0)
		{
			newModelID = myFreeModelIDs.Pop();
		}
		else
		{
			newModelID = myModelList.Size();
			myModelList.Add();
		}

		myModels[aModelPath] = newModelID;

		if (CreateModel(aModelPath, newModelID) == false)
		{
			myModelList.Pop();
			myModels.erase(aModelPath);
			return NULL_MODEL;
		}

		LoadAnimations(aModelPath, newModelID);
	}

	myModelList[myModels[aModelPath]].AddRef();
	return myModels[aModelPath];
}

const CModelManager::ModelId CModelManager::LoadGUIModel(const CLoaderMesh* aLoaderMesh, const char* aTexturePath)
{
	DL_MESSAGE_BOX("LoadGUIModel function is not compatible with engine updates in BrontosaurusGore");
	return -1;
#if 0
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

		myModelList[handle].Initialize(effect, surface, aLoaderMesh);

		SPixelConstantBuffer bufferStruct = {};
		ID3D11Buffer* pixelConstantBuffer = BSR::CreateCBuffer<SPixelConstantBuffer>(&bufferStruct);
		myModelList[handle].AddConstantBuffer(CModel::eShaderStage::ePixel, pixelConstantBuffer);
	}

	return myModels[aLoaderMesh->myName];
#endif
}

const CModelManager::ModelId CModelManager::LoadGUIModel(const CLoaderMesh* aLoaderMesh, const CU::GrowingArray<std::string>& aTexturePaths)
{
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

const std::string& CModelManager::GetFilePath(const ModelId aModelID) const
{
	for (const auto& model : myModels)
	{
		if (model.second == aModelID)
		{
			return model.first;
		}
	}

	static const std::string NullString;
	return NullString;
}

void CModelManager::LoadAnimations(const std::string& aPath, const ModelId aModelId)
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

			if (animationScene->mNumMaterials > 1)
			{
				DL_MESSAGE_BOX("More than one material found in animation fbx scene\n%s", aPath.c_str());
			}

			mdl->mySceneAnimators[static_cast<eAnimationState>(i)] = CSceneAnimator();
			mdl->mySceneAnimators[static_cast<eAnimationState>(i)].Init(animationScene);

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
			if (mdl->mySceneAnimator->CurrentAnimIndex != -1)
			{
				mdl->mySceneAnimator->PlayAnimationForward();
			}
			else
			{
				DL_MESSAGE_BOX("No animation found in %s", aPath.c_str());
				mdl->mySceneAnimator = nullptr;
			}
		}
	}
}

bool CModelManager::CreateModel(const std::string& aModelPath, ModelId aNewModel)
{
	return CModelLoader::LoadModel(aModelPath.c_str(), &myModelList[aNewModel]/*tempModelPointer*/);
}

bool CModelManager::DoesModelExists(const std::string& aModelPath)
{
	return myModels.find(aModelPath) != myModels.end();
}

