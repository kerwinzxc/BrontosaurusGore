#pragma once
#include <unordered_map>
#include <Stack.h>
#include <string>
#include "Model.h"
#include "ModelShapes.h"

namespace CU
{
	class ThreadPool;
	class DynamicString;
	//class Matrix44f;
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

	const ModelId LoadModel(const CU::DynamicString& aModelPath);
	const ModelId LoadModel(const SShape aModelShape);
	const ModelId LoadGUIModel(const CLoaderMesh* aLoaderMesh, const char* aTexturePath);
	const ModelId LoadGUIModel(const CLoaderMesh* aLoaderMesh, const CU::GrowingArray<std::string>& aTexturePaths);
	
	CModel* GetModel(const ModelId aModelID);
	
	void RemoveModel(const ModelId aModelID);


private:
	void LoadAnimations(const char * aPath, const ModelId aModelId);
	bool CreateModel(const CU::DynamicString& aModelPath, ModelId aNewModel);
	CModel* CreateShape(const SShape aModelShape);
	
	bool DoesModelExists(const char * aModelPath);
	bool DoesShapeExist(SShape aModelShape);

private:
	std::unordered_map<std::string, ModelId> myModels;
	std::unordered_map<eModelShape, ModelId> myShapes;

	// not in Retail?
	std::unordered_map<float, ModelId> mySpheres;

	CU::Stack<ModelId> myFreeModelIDs;

	CU::GrowingArray<CModel, ModelId> myModelList;

	CU::ThreadPool* myThreadPool;

	static const unsigned int ourMaxNumberOfModels = 512u;
};

