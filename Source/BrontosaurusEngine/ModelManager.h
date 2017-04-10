#pragma once
#include <unordered_map>
#include <Stack.h>
#include <string>
#include "Model.h"

namespace CU
{
	class ThreadPool;
	class DynamicString;
}

class CModel;
class CSprite;
class CModelInstance;
class CSpriteInstance;

#define NULL_MODEL -1

class CModelManager
{
public:
	typedef int ModelId;
	
	CModelManager();
	~CModelManager();

	const ModelId LoadModel(const std::string& aModelPath);
	const ModelId LoadGUIModel(const CLoaderMesh* aLoaderMesh, const char* aTexturePath);
	const ModelId LoadGUIModel(const CLoaderMesh* aLoaderMesh, const CU::GrowingArray<std::string>& aTexturePaths);
	CModel* GetModel(const ModelId aModelID);
	void RemoveModel(const ModelId aModelID);
	int GetModelRefCount(const ModelId aModelID) const;
	const std::string& GetFilePath(const ModelId aModelID) const;

private:
	void LoadAnimations(const std::string& aPath, const ModelId aModelId);
	bool CreateModel(const std::string& aModelPath, ModelId aNewModel);
	bool DoesModelExists(const std::string& aModelPath);

private:
	std::unordered_map<std::string, ModelId> myModels;
	CU::Stack<ModelId> myFreeModelIDs;
	CU::GrowingArray<CModel, ModelId> myModelList;
};

