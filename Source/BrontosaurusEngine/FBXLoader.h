#pragma once
#include "VertexStructs.h"
#include <vector>
#include <map>

#include "../CommonUtilities/matrix44.h"

#include "SphereColData.h"
#include "FbxLoader/AssImp/scene.h"

struct BoneInfo
{
	FBXLoader::Matrix44f BoneOffset;
	FBXLoader::Matrix44f FinalTransformation;

	BoneInfo()
	{

	}
};

class CLoaderCamera
{
public:
	CU::Matrix44f myTransformation;
	float myFar;
	float myNear;
	float myFov;
	float myAspectRatio;
};

class CLoaderMesh;
class CSceneAnimator;

class CLoaderScene
{
public:
	CLoaderScene() : myMeshes(2), myTextures(8), isLODScene(false), myScene(nullptr) {}
	~CLoaderScene() { myMeshes.DeleteAll(); SAFE_DELETE(myCamera); }
	std::string myAlbedoTexture;
	CU::GrowingArray<std::string> myTextures;
	CU::GrowingArray<CLoaderMesh*> myMeshes;
	CLoaderCamera* myCamera;
	const struct aiScene* myScene;

	SSphereColData mySphereColData;
	bool isLODScene;

	// Animation data
	std::vector<BoneInfo> myBoneInfo;
	std::map<std::string, unsigned int> myBoneNameToIndex;
	unsigned int myNumBones;
};

// One model can contain multiple meshes
class CLoaderMesh // TODO: maybey memory leeks?
{
public:
	CLoaderMesh() 
	{
		myShaderType = 0; 
		myVerticies = nullptr; 
		myVertexBufferSize = 0; 
		myVertexCount = 0; 
		myModel = nullptr;
		myScene = nullptr;
		myLOD_DistStart = myLOD_DistEnd = -1.0f;
		myLODLevel = -1;
	}
	~CLoaderMesh() { delete[] myVerticies; myVerticies = nullptr; }
	std::vector<unsigned int> myIndexes;
	std::vector<CLoaderMesh*> myChildren;
	CU::Matrix44f myTransformation;
	unsigned int myShaderType;
	unsigned int myVertexBufferSize; //hur stor en vertex är
	int myVertexCount;
	class CLoaderModel* myModel;
	class CLoaderScene* myScene;
	char* myVerticies; //datan
	const char* myName;
	CU::Vector3f myMaxPoint;
	CU::Vector3f myMinPoint;

	bool myIsAlphaMesh;

	std::string myShaderFile;

	int myLODLevel;
	float myLOD_DistStart;
	float myLOD_DistEnd;

	

};

class CLoaderModel
{
public:
	CLoaderModel(){ myIsLoaded = false; myAnimationDuration = 0.0f; }
	~CLoaderModel(){}
	void SetData(const char* aModelPath){ myModelPath = aModelPath; }
	CLoaderMesh* CreateMesh(){ CLoaderMesh *model = new CLoaderMesh(); myMeshes.push_back(model); model->myModel = this; return model; }

	std::vector<CLoaderMesh*> myMeshes;
	std::string myModelPath;
	float myAnimationDuration;
	const struct aiScene* myScene;
	FBXLoader::Matrix44f myGlobalInverseTransform;
	bool myIsLoaded;
	std::vector<std::string> myTextures;
	//// Animation data
	//std::vector<BoneInfo> myBoneInfo;
	//std::map<std::string, unsigned int> myBoneNameToIndex;
	//unsigned int myNumBones;


};

struct aiNode;
class CFBXLoader
{
public:
	CFBXLoader();
	~CFBXLoader();

	//Consider Using LoadModelScene instead
	CLoaderModel *LoadModel(const char* aModel);
	bool LoadGUIScene(const char * aFilePath, CLoaderScene & aSceneOut);
	bool LoadModelScene(const char * aFilePath, CLoaderScene & aSceneOut);
	const struct aiScene* GetScene(const std::string& aFBXPath);


private:
	void LoadMeshChildren(aiNode* aNode, CU::GrowingArray<aiNode*>& aNodesOut);

	void* LoadModelInternal(CLoaderModel* someInput);
	int DetermineAndLoadVerticies(struct aiMesh* aMesh, CLoaderMesh* aLoaderMesh);
	void LoadMaterials(const struct aiScene *sc, CLoaderModel* aModel);
	void LoadTexture(int aType, std::vector<std::string>& someTextures, struct aiMaterial* aMaterial);

};

