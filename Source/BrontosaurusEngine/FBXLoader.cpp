#include "stdafx.h"
#ifdef new
#undef new
#endif // new

#include "FBXLoader.h"

#include <FbxLoader\AssImp\cimport.h>
#include <FbxLoader\assimp\scene.h>
#include <FbxLoader\assimp\postprocess.h>

#include "Engine.h"

#include <map>
#include <string>
#include <windows.h>
#include <fstream>
#include "Model.h"

#include <memory.h>

// Change to your path
#ifdef _DEBUG
#pragma comment (lib, "..\\CommonUtilities\\FbxLoader\\assimp-vc140-mtd.lib")
#else
#pragma comment (lib, "..\\CommonUtilities\\FbxLoader\\assimp-vc140-mt.lib")
#endif

#define TEXTURE_SET_0 0

const float HardCodedFovConstant = 39.f * (3.14f / 180.f) * 0.8f;
#define BASE_SCALE 100

CFBXLoader::CFBXLoader()
{
}


CFBXLoader::~CFBXLoader() 
{
}

FBXLoader::Matrix44f ConvertToEngineMatrix33(const aiMatrix3x3& AssimpMatrix)
{

	FBXLoader::Matrix44f mat;
	mat.myMatrix[0][0] = AssimpMatrix.a1;	mat.myMatrix[0][1] = AssimpMatrix.a2;	mat.myMatrix[0][2] = AssimpMatrix.a3;	mat.myMatrix[0][3] = 0.0f;
	mat.myMatrix[1][0] = AssimpMatrix.b1;	mat.myMatrix[1][1] = AssimpMatrix.b2;	mat.myMatrix[1][2] = AssimpMatrix.b3;	mat.myMatrix[1][3] = 0.0f;
	mat.myMatrix[2][0] = AssimpMatrix.c1;	mat.myMatrix[2][1] = AssimpMatrix.c2;	mat.myMatrix[2][2] = AssimpMatrix.c3;	mat.myMatrix[2][3] = 0.0f;
	mat.myMatrix[3][0] = 0.0f;				mat.myMatrix[3][1] = 0.0f;				mat.myMatrix[3][2] = 0.0f;				mat.myMatrix[3][3] = 1.0f;
	return mat;
}

// constructor from Assimp matrix
FBXLoader::Matrix44f ConvertToEngineMatrix44(const aiMatrix4x4& AssimpMatrix)
{
	FBXLoader::Matrix44f mat;
	mat.myMatrix[0][0] = AssimpMatrix.a1; mat.myMatrix[0][1] = AssimpMatrix.a2; mat.myMatrix[0][2] = AssimpMatrix.a3; mat.myMatrix[0][3] = AssimpMatrix.a4;
	mat.myMatrix[1][0] = AssimpMatrix.b1; mat.myMatrix[1][1] = AssimpMatrix.b2; mat.myMatrix[1][2] = AssimpMatrix.b3; mat.myMatrix[1][3] = AssimpMatrix.b4;
	mat.myMatrix[2][0] = AssimpMatrix.c1; mat.myMatrix[2][1] = AssimpMatrix.c2; mat.myMatrix[2][2] = AssimpMatrix.c3; mat.myMatrix[2][3] = AssimpMatrix.c4;
	mat.myMatrix[3][0] = AssimpMatrix.d1; mat.myMatrix[3][1] = AssimpMatrix.d2; mat.myMatrix[3][2] = AssimpMatrix.d3; mat.myMatrix[3][3] = AssimpMatrix.d4;
	return mat;
}

CU::Matrix44f ConvertToCUMatrix44(const aiMatrix4x4& aAssimpMatrix)
{
	CU::Matrix44f mat;
	mat.my2DArray[0][0] = aAssimpMatrix.a1; mat.my2DArray[0][1] = aAssimpMatrix.a2; mat.my2DArray[0][2] = aAssimpMatrix.a3; mat.my2DArray[0][3] = aAssimpMatrix.a4;
	mat.my2DArray[1][0] = aAssimpMatrix.b1; mat.my2DArray[1][1] = aAssimpMatrix.b2; mat.my2DArray[1][2] = aAssimpMatrix.b3; mat.my2DArray[1][3] = aAssimpMatrix.b4;
	mat.my2DArray[2][0] = aAssimpMatrix.c1; mat.my2DArray[2][1] = aAssimpMatrix.c2; mat.my2DArray[2][2] = aAssimpMatrix.c3; mat.my2DArray[2][3] = aAssimpMatrix.c4;
	mat.my2DArray[3][0] = aAssimpMatrix.d1; mat.my2DArray[3][1] = aAssimpMatrix.d2; mat.my2DArray[3][2] = aAssimpMatrix.d3; mat.my2DArray[3][3] = aAssimpMatrix.d4;
	return mat;
}

int CFBXLoader::DetermineAndLoadVerticies(aiMesh* fbxMesh, CLoaderMesh* aLoaderMesh)
{
	unsigned int modelBluePrintType = 0;

	modelBluePrintType |= (fbxMesh->HasPositions() ? EModelBluePrint_Position : 0);
	modelBluePrintType |= (fbxMesh->HasTextureCoords(0) ? EModelBluePrint_UV : 0);
	modelBluePrintType |= (fbxMesh->HasNormals() ? EModelBluePrint_Normal : 0);
	modelBluePrintType |= (fbxMesh->HasTangentsAndBitangents() ? EModelBluePrint_BinormTan : 0);
	modelBluePrintType |= (fbxMesh->HasBones() ? EModelBluePrint_Bones : 0);

	int vertexBufferSize = 0;
	vertexBufferSize += (fbxMesh->HasPositions() ? sizeof(float) * 4 : 0);
	vertexBufferSize += (fbxMesh->HasTextureCoords(0) ? sizeof(float) * 2 : 0);
	vertexBufferSize += (fbxMesh->HasNormals() ? sizeof(float) * 4 : 0);
	vertexBufferSize += (fbxMesh->HasTangentsAndBitangents() ? sizeof(float) * 8 : 0);
	vertexBufferSize += (fbxMesh->HasBones() ? sizeof(float) * 4 * 2 : 0); // Better with an UINT, but this works

	aLoaderMesh->myShaderType = modelBluePrintType;
	aLoaderMesh->myVertexBufferSize = vertexBufferSize;

	aLoaderMesh->myVerticies = new char[vertexBufferSize * fbxMesh->mNumVertices];
	aLoaderMesh->myVertexCount = fbxMesh->mNumVertices;


	std::vector<VertexBoneData> collectedBoneData;
	if (fbxMesh->HasBones() && aLoaderMesh->myScene != nullptr)
	{
		collectedBoneData.resize(fbxMesh->mNumVertices);

		unsigned int BoneIndex = 0;
		for (unsigned int i = 0; i < fbxMesh->mNumBones; i++) 
		{
			std::string BoneName(fbxMesh->mBones[i]->mName.data);
			if (aLoaderMesh->myScene->myBoneNameToIndex.find(BoneName) == aLoaderMesh->myScene->myBoneNameToIndex.end())
			{
				BoneIndex = aLoaderMesh->myScene->myNumBones;
				aLoaderMesh->myScene->myNumBones++;
				BoneInfo bi;
				aLoaderMesh->myScene->myBoneInfo.push_back(bi);

				
				FBXLoader::Matrix44f NodeTransformation = ConvertToEngineMatrix44(fbxMesh->mBones[i]->mOffsetMatrix);

				aLoaderMesh->myScene->myBoneInfo[BoneIndex].BoneOffset = NodeTransformation;
				aLoaderMesh->myScene->myBoneNameToIndex[BoneName] = BoneIndex;
			}
			else
			{
				BoneIndex = aLoaderMesh->myScene->myBoneNameToIndex[BoneName];
			}

			for (unsigned int j = 0; j < fbxMesh->mBones[i]->mNumWeights; j++) 
			{
				unsigned int VertexID = fbxMesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = fbxMesh->mBones[i]->mWeights[j].mWeight;
				collectedBoneData[VertexID].AddBoneData(BoneIndex, Weight);
			}
		}
	}
	

	SVertexCollection vertexCollection;
	for (unsigned int i = 0; i < fbxMesh->mNumVertices; i++)
	{
		if (fbxMesh->HasPositions())
		{
			aiVector3D& mVertice = fbxMesh->mVertices[i];
			vertexCollection.PushVec4(FBXLoader::Vector4f(mVertice.x, mVertice.y, mVertice.z, 1));
		}
		if (fbxMesh->HasNormals())
		{
			aiVector3D& mNorm = fbxMesh->mNormals[i];
			vertexCollection.PushVec4(FBXLoader::Vector4f(mNorm.x, mNorm.y, mNorm.z, 1));
		}
		if (fbxMesh->HasTangentsAndBitangents())
		{
			aiVector3D& mTangent = fbxMesh->mTangents[i];
			aiVector3D& biTangent = fbxMesh->mBitangents[i];

			vertexCollection.PushVec4(FBXLoader::Vector4f(mTangent.x, mTangent.y, mTangent.z, 1));
			vertexCollection.PushVec4(FBXLoader::Vector4f(biTangent.x, biTangent.y, biTangent.z, 1));
		}
		if (fbxMesh->HasTextureCoords(TEXTURE_SET_0))		//HasTextureCoords(texture_coordinates_set)
		{
			vertexCollection.PushVec2(FBXLoader::Vector2f(fbxMesh->mTextureCoords[TEXTURE_SET_0][i].x, fbxMesh->mTextureCoords[TEXTURE_SET_0][i].y));
		}
		if (fbxMesh->HasBones())
		{
			VertexBoneData& boneData = collectedBoneData[i];

			//TODO: fix so vertexCollexion can haz UINTS too so we don't need to cast float to uint in the sha-ding
			//TODO: see VertexCollexion, i added code but we have to change in shader too before start using different stuffs mvh carl
			// UINTS woudl be better
			aiVector3D bones;
			vertexCollection.PushVec4(FBXLoader::Vector4f((float)boneData.IDs[0], (float)boneData.IDs[1], (float)boneData.IDs[2], (float)boneData.IDs[3])); //TODO: change to uint, change in shader too

			aiVector3D weights;
			vertexCollection.PushVec4(FBXLoader::Vector4f(boneData.Weights[0], boneData.Weights[1], boneData.Weights[2], boneData.Weights[3]));
		}
	}

	memcpy(aLoaderMesh->myVerticies, &vertexCollection.myData[0], vertexBufferSize * fbxMesh->mNumVertices);

	return vertexBufferSize;
}

CLoaderModel* CFBXLoader::LoadModel(const std::string& aModel)
{
	CLoaderModel* newModel = new CLoaderModel();
	newModel->SetData(aModel);

	if (!LoadModelInternal(newModel))
	{
		delete newModel;
		newModel = nullptr;
	}

	return newModel;
}

bool does_file_exist(const std::string& fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

CLoaderCamera* ParseCamera(const aiCamera* aCameraIn)
{
	if (aCameraIn == nullptr)
	{
		return nullptr;
	}

	CLoaderCamera* cameraOut = new CLoaderCamera();

	CU::Vector3f upVector = -CU::Vector3f(aCameraIn->mUp.x, aCameraIn->mUp.y, aCameraIn->mUp.z); //fulhax bc det blev upp och ner (operator-() på vektorn dvs)
	upVector.Normalize();
	CU::Vector3f forwardVector = CU::Vector3f(aCameraIn->mLookAt.x, aCameraIn->mLookAt.y, aCameraIn->mLookAt.z);
	forwardVector.Normalize();
	CU::Vector3f rightVector = (upVector.Cross(forwardVector));
	rightVector.Normalize();
	CU::Vector3f positionVector = CU::Vector3f(aCameraIn->mPosition.x, aCameraIn->mPosition.y, aCameraIn->mPosition.z);

	cameraOut->myTransformation.myUpVector = upVector;
	cameraOut->myTransformation.myRightVector = rightVector;
	cameraOut->myTransformation.myForwardVector = forwardVector;

	cameraOut->myTransformation.myPosition = positionVector;
	
	cameraOut->myFar = aCameraIn->mClipPlaneFar;
	cameraOut->myNear = aCameraIn->mClipPlaneNear;
	cameraOut->myFov = HardCodedFovConstant; // aCameraIn->mHorizontalFOV;
	cameraOut->myAspectRatio = aCameraIn->mAspect;

	return cameraOut;
}

bool ExtractBaseName(std::string& aFilePathInOut)
{
	size_t index = aFilePathInOut.find_last_of("\\");
	if (index == std::string::npos)
	{
		index = aFilePathInOut.find_last_of("/");
		if (index == std::string::npos)
		{
			return false;
		}
	}

	aFilePathInOut.erase(aFilePathInOut.begin(), aFilePathInOut.begin() + index + 1);
	return true;
}

bool AddFullPath(std::string aModelPath, std::string& aFilePathInOut)
{
	size_t index = aModelPath.find_last_of("\\");
	if (index == std::string::npos)
	{
		index = aModelPath.find_last_of("/");
		if (index == std::string::npos)
		{
			return false;
		}
	}

	aModelPath.erase(aModelPath.begin() + index + 1, aModelPath.end());
	aFilePathInOut.insert(0, aModelPath);
	return true;
}

void DetermineBoxCollider(const aiVector3D* aVertices, const unsigned int aVertexCount, CLoaderMesh& aLoaderMesh)
{
	CU::Point3f minPoint(aVertices[0].x, aVertices[0].y, aVertices[0].z);
	CU::Point3f maxPoint(aVertices[aVertexCount - 1].x, aVertices[aVertexCount - 1].y, aVertices[aVertexCount - 1].z);

	for (unsigned int i = 0; i < aVertexCount; ++i) //could probably be sped up by else-if'ing max after min, mebe, try it :). -Kyle
	{
		if (aVertices[i].x <= minPoint.x)
		{
			minPoint.x = aVertices[i].x;
		}
		if (aVertices[i].y <= minPoint.y)
		{
			minPoint.y = aVertices[i].y;
		}
		if (aVertices[i].z <= minPoint.z)
		{
			minPoint.z = aVertices[i].z;
		}
		if (aVertices[i].x >= maxPoint.x)
		{
			maxPoint.x = aVertices[i].x;
		}
		if (aVertices[i].y >= maxPoint.y)
		{
			maxPoint.y = aVertices[i].y;
		}
		if (aVertices[i].z >= maxPoint.z)
		{
			maxPoint.z = aVertices[i].z;
		}
	}

	aLoaderMesh.myMaxPoint = maxPoint;
	aLoaderMesh.myMinPoint = minPoint;
}

const aiNode* FindTranslationNode(const aiNode* aNode)
{
	std::string translationName = aNode->mName.C_Str();
	while (translationName.find("Translation") == std::string::npos)
	{
		aNode = aNode->mParent;
		if (aNode == nullptr)
		{
			return nullptr;
		}

		translationName = aNode->mName.C_Str();
	}

	return aNode;
}

const aiNode* FindLODDNode(const aiNode* aNode)
{
	std::string translationName = aNode->mName.C_Str();
	while (translationName.find("LOD") == std::string::npos)
	{
		aNode = aNode->mParent;
		if (aNode == nullptr)
		{
			return nullptr;
		}

		translationName = aNode->mName.C_Str();
	}

	return aNode;
}

bool CFBXLoader::LoadGUIScene(const std::string& aFilePath, CLoaderScene& aSceneOut)
{
	if (does_file_exist(aFilePath) == false)
	{
		//DL_MESSAGE_BOX("fbx file not found: %s", aFilePath);
		return false;
	}

	const aiScene* scene = aiImportFile(aFilePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	aSceneOut.myScene = scene;
	if (scene->mNumCameras < 1)
	{
		DL_MESSAGE_BOX("fbx file has no camera: %s", aFilePath.c_str());
		return false;
	}

	if (scene->mNumCameras > 1)
	{
		DL_MESSAGE_BOX("fbx file has more than one camera: %s\n(Carl don't know what to do with this)", aFilePath);
		return false;
	}

	aiCamera* camera = scene->mCameras[0];
	aSceneOut.myCamera = ParseCamera(camera);

	aiNode* root = scene->mRootNode;
	if (root == nullptr)
	{
		DL_MESSAGE_BOX("fbx file has no root node: %s", aFilePath.c_str());
		return false;
	}

	if (root->mNumChildren == 0)
	{
		DL_MESSAGE_BOX("fbx file root node is empty or massive: %s", aFilePath.c_str());
		return false;
	}

	aiNode* modelGroup = nullptr;
	for (unsigned int i = 0; i < root->mNumChildren; ++i)
	{
		std::string name = root->mChildren[i]->mName.C_Str();
		if (name.find("Grp") != std::string::npos || name.find("Base") != std::string::npos || name.find("shop") != std::string::npos || name.find("Shop") != std::string::npos || name.find("buy") != std::string::npos) //TODO: Solv this hell
		{
			modelGroup = root->mChildren[i];
			break;
		}
	}

	if (root->mNumChildren >= 3)
	{
		modelGroup = root;
	}
	
	CU::GrowingArray<aiNode*> nodes(4);
	LoadMeshChildren(modelGroup, nodes);
	for (unsigned int i = 0; i < nodes.Size(); ++i)
	{
		unsigned int meshIndex = nodes[i]->mMeshes[0];
		aiMesh* fbxMesh = scene->mMeshes[meshIndex];

		CLoaderMesh* mesh = new CLoaderMesh();
		const aiNode* translationNode = FindTranslationNode(nodes[i]);
		if (translationNode != nullptr)
		{
			mesh->myTransformation = ConvertToCUMatrix44(translationNode->mTransformation).Transpose();
		}
		else
		{
			mesh->myTransformation = ConvertToCUMatrix44(nodes[i]->mTransformation).Transpose();
		}

		DetermineAndLoadVerticies(fbxMesh, mesh);

		for (unsigned int j = 0; j < fbxMesh->mNumFaces; j++)
		{
			for (uint k = 0; k < fbxMesh->mFaces[j].mNumIndices; k++)
			{
				mesh->myIndexes.push_back(fbxMesh->mFaces[j].mIndices[k]);
			}
		}

		DetermineBoxCollider(fbxMesh->mVertices, fbxMesh->mNumVertices, *mesh);

		mesh->myName = nodes[i]->mName.C_Str();
		aSceneOut.myMeshes.Add(mesh);
	}
	
	if (scene->HasMaterials() == true)
	{
		aiString path;
		aiReturn result = scene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		if (result != aiReturn_SUCCESS)
		{
			DL_MESSAGE_BOX("Found no albedo texture at path %s", aFilePath.c_str());
		}

		aSceneOut.myAlbedoTexture = path.C_Str();
		ExtractBaseName(aSceneOut.myAlbedoTexture);
		AddFullPath(aFilePath, aSceneOut.myAlbedoTexture);

		CLoaderModel temp;
		LoadMaterials(scene, &temp);
		for (size_t i = 0; i < temp.myTextures.size(); ++i)
		{
			if (temp.myTextures[i] != "")
			{
				//ExtractBaseName(temp.myTextures[i]);
				AddFullPath(aFilePath, temp.myTextures[i]);
			}
			aSceneOut.myTextures.Add(temp.myTextures[i]);
		}
	}

	return true;
}

bool CFBXLoader::LoadModelScene(const std::string& aFilePath, CLoaderScene& aSceneOut)
{
	if (does_file_exist(aFilePath) == false)
	{
		//RESOURCES_LOG("fbx file not found: %s", aFilePath);
		//DL_PRINT_WARNING("fbx file not found: %s", aFilePath);
		return false;
	}

	const aiScene* scene = aiImportFile(aFilePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded /*| aiProcess_Debone*/); //DEBONE GJORDE SÅ VAPNENA INTE FICK NÅGRA BEN
	aSceneOut.myScene = scene;
	aSceneOut.myCamera = nullptr;

	aiNode* root = scene->mRootNode;
	if (root == nullptr)
	{
		DL_ASSERT("fbx file has no root node: %s", aFilePath.c_str());
		return false;
	}

	if (root->mNumChildren == 0)
	{
		DL_ASSERT("fbx file root node is empty or massive: %s", aFilePath.c_str());
		return false;
	}

	if (scene->mNumMaterials > 1)
	{
		DL_MESSAGE_BOX("fbx scene contains more than one material, this will split the mesh which the engine does not support\n%s", aFilePath.c_str());
	}

	CU::GrowingArray<aiNode*> nodes(4);
	LoadMeshChildren(root, nodes);
	for (unsigned int i = 0; i < nodes.Size(); ++i)
	{
		unsigned int last = (nodes[i]->mNumMeshes > 0) ? nodes[i]->mNumMeshes - 1 : 0;
		unsigned int meshIndex = nodes[i]->mMeshes[last];
		aiMesh* fbxMesh = scene->mMeshes[meshIndex];

		CLoaderMesh* mesh = new CLoaderMesh();
		mesh->myScene = &aSceneOut;
		const aiNode* translationNode = FindTranslationNode(nodes[i]);
		if (translationNode != nullptr)
		{
			mesh->myTransformation = ConvertToCUMatrix44(translationNode->mTransformation).Transpose();
		}

		//LOD DATA
		const aiNode* lodNode = FindLODDNode(nodes[i]);
		if (lodNode != nullptr)
		{
			aSceneOut.isLODScene = true;
			lodNode->mMetaData->Get<float>("LOD_distStart", mesh->myLOD_DistStart);
			lodNode->mMetaData->Get<float>("LOD_distEnd", mesh->myLOD_DistEnd);

			mesh->myLODLevel = i;
		}

		aiString shaderFile;
		shaderFile = "";
		if (nodes[i]->mMetaData->Get<aiString>("customShader", shaderFile))
		{
			mesh->myShaderFile = shaderFile.C_Str();
		}

		bool alpha;
		if (nodes[i]->mMetaData->Get<bool>("AlphaMaterial", alpha))
		{
			mesh->myIsAlphaMesh = alpha;
		}




		DetermineAndLoadVerticies(fbxMesh, mesh);

		for (unsigned int j = 0; j < fbxMesh->mNumFaces; j++)
		{
			for (uint k = 0; k < fbxMesh->mFaces[j].mNumIndices; k++)
			{
				mesh->myIndexes.push_back(fbxMesh->mFaces[j].mIndices[k]);
			}
		}

		DetermineBoxCollider(fbxMesh->mVertices, fbxMesh->mNumVertices, *mesh);

		mesh->myName = nodes[i]->mName.C_Str();
		aSceneOut.myMeshes.Add(mesh);
	}




	if (scene->HasMaterials() == true)
	{
		aiString path;
		aiReturn result = scene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		if (result != aiReturn_SUCCESS)
		{
			//DL_ASSERT("Failed to get diffuse/albedo texture from fbx scene: %s", aFilePath.c_str());

			//return false;
		}

		aSceneOut.myAlbedoTexture = path.C_Str();
		ExtractBaseName(aSceneOut.myAlbedoTexture);
		AddFullPath(aFilePath, aSceneOut.myAlbedoTexture);

		CLoaderModel temp;
		LoadMaterials(scene, &temp);
		for (size_t i = 0; i < temp.myTextures.size(); ++i)
		{
			aSceneOut.myTextures.Add(temp.myTextures[i].c_str());
		}
	}

	return true;
}

const aiScene* CFBXLoader::GetScene(const std::string& aFBXPath)
{
	if (!does_file_exist(aFBXPath.c_str()))
	{
		return nullptr;
	}

	return aiImportFile(aFBXPath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
}


void CFBXLoader::LoadMeshChildren(aiNode* aNode, CU::GrowingArray<aiNode*>& aNodesOut)
{
	if (aNode == nullptr)
	{
		return;
	}

	if (aNode->mNumMeshes != 0)
	{
		aNodesOut.Add(aNode);
	}

	for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
	{
		aiNode* child = aNode->mChildren[i];
		static const std::string animationIdentifier = "SKEL";
		static const std::string animationIdentifier2 = "SKIN";
		std::string nodeName = aNode->mName.C_Str();
		if (nodeName.find(animationIdentifier) != std::string::npos)
		{
			//continue;
		}
		if (nodeName.find(animationIdentifier2) != std::string::npos)
		{
			//continue;
		}
		LoadMeshChildren(child, aNodesOut);
	}
}

void* CFBXLoader::LoadModelInternal(CLoaderModel* someInput)
{
	CLoaderModel* model = someInput;
	const struct aiScene* scene = NULL;

	if (!does_file_exist(model->myModelPath.c_str()))
	{
		DL_MESSAGE_BOX("File not found: %s", model->myModelPath.c_str());
		//MessageBox(nullptr, longModelPath.c_str(), L"Load error", MB_OK | MB_ICONERROR);
		OutputDebugStringA("File not found");
		return nullptr;
	}
	scene = aiImportFile(model->myModelPath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

	OutputDebugStringA(model->myModelPath.c_str());
	
	if (!scene)
	{
		OutputDebugStringA(aiGetErrorString());
		return nullptr;
	}

	model->myScene = scene;
	
	for (unsigned int n = 0; n < scene->mNumMeshes; ++n)
	{
		CLoaderMesh* mesh = model->CreateMesh();
		mesh->myName = someInput->myModelPath.c_str();

		aiMesh* fbxMesh = scene->mMeshes[n];

		DetermineAndLoadVerticies(fbxMesh, mesh);
		DetermineBoxCollider(fbxMesh->mVertices, fbxMesh->mNumVertices, *mesh);

		for (unsigned int i = 0; i < fbxMesh->mNumFaces; i++)
		{
			for (uint j = 0; j < fbxMesh->mFaces[i].mNumIndices; j++)
			{
				mesh->myIndexes.push_back(fbxMesh->mFaces[i].mIndices[j]);
			}
		}
	}

	// CHange to support multiple animations
	if (scene->mNumAnimations > 0)
	{
		model->myAnimationDuration = (float)scene->mAnimations[0]->mDuration;
	}
	
	LoadMaterials(scene, model);

	model->myGlobalInverseTransform = ConvertToEngineMatrix44(scene->mRootNode->mTransformation);

	return model;

}

void CFBXLoader::LoadMaterials(const struct aiScene *sc, CLoaderModel* aModel)
{
	for (unsigned int m = 0; m < sc->mNumMaterials; m++)
	{
		LoadTexture(aiTextureType_DIFFUSE, aModel->myTextures, sc->mMaterials[m]); // TEXTURE_DEFINITION_ALBEDO
		LoadTexture(aiTextureType_SPECULAR, aModel->myTextures, sc->mMaterials[m]); // TEXTURE_DEFINITION_ROUGHNESS
		LoadTexture(aiTextureType_AMBIENT, aModel->myTextures, sc->mMaterials[m]); // TEXTURE_DEFINITION_AMBIENTOCCLUSION
		LoadTexture(aiTextureType_EMISSIVE, aModel->myTextures, sc->mMaterials[m]); // TEXTURE_DEFINITION_EMISSIVE
		LoadTexture(aiTextureType_HEIGHT, aModel->myTextures, sc->mMaterials[m]);
		LoadTexture(aiTextureType_NORMALS, aModel->myTextures, sc->mMaterials[m]); // TEXTURE_DEFINITION_NORMAL
		LoadTexture(aiTextureType_SHININESS, aModel->myTextures, sc->mMaterials[m]);
		LoadTexture(aiTextureType_OPACITY, aModel->myTextures, sc->mMaterials[m]);
		LoadTexture(aiTextureType_DISPLACEMENT, aModel->myTextures, sc->mMaterials[m]);
		LoadTexture(aiTextureType_LIGHTMAP, aModel->myTextures, sc->mMaterials[m]);
		LoadTexture(aiTextureType_REFLECTION, aModel->myTextures, sc->mMaterials[m]); // TEXTURE_DEFINITION_METALNESS

		if (sc->mNumMaterials > 1)
		{
			bool gotNothing = true;
			for (std::string& path : aModel->myTextures)
			{
				if (path.empty() == false)
				{
					gotNothing = false;
				}
			}

			if (gotNothing == true)
			{
				aModel->myTextures.clear();
			}
		}
	}
}

void CFBXLoader::LoadTexture(int aType, std::vector<std::string>& someTextures, aiMaterial* aMaterial)
{
	int texIndex = 0;
	aiReturn texFound = AI_SUCCESS;

	aiString path;	// filename

	texFound = aMaterial->GetTexture((aiTextureType)aType, texIndex, &path);
	if (texFound == AI_FAILURE)
	{
		someTextures.push_back("");
		return;
	}


	std::string filePath = std::string(path.data);

	const size_t last_slash_idx = filePath.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
	{
		filePath.erase(0, last_slash_idx + 1);
	}

	someTextures.push_back(filePath);
}
