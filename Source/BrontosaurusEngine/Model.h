#pragma once
#include <atomic>
#include "BufferStructs.h"
#include "SphereColData.h"
#include "AnimationController.h"

#include "../CommonUtilities/StaticArray.h"
#include "../CommonUtilities/AABB.h"
#include "stdafx.h"

class CEffect;
class CSurface;
class CDXFramework;
struct ID3D11Buffer;
struct SVertexData;
struct SConstLayout;
class CLoaderModel;
class CLoaderMesh;

class CSceneAnimator;
class CModelInstance;
class CModelLoader;

namespace Lights
{
	struct SDirectionalLight;
	class matrix44;
}

namespace CU
{
	class Camera;
}


class CPointLightInstance;

struct SLodData
{
	SLodData() { myLodLevel = myIndexCount = myVertexCount = 0; myVertexBuffer = myIndexBuffer = nullptr; }
	~SLodData() { /*SAFE_RELEASE(myVertexBuffer); SAFE_RELEASE(myIndexBuffer);*/ }
	SLODDistances myDistances;		  
	ID3D11Buffer* myVertexBuffer;	  
	ID3D11Buffer* myIndexBuffer;	  
									  
	int myLodLevel;		
	unsigned int myVertexCount;		  
	unsigned int myIndexCount;		  


};


class CModel
{
	friend class CModelLoader;
	friend class CModelManager;

public:
	enum class eShaderStage
	{
		eVertex,
		eGeometry,
		ePixel,
		eLength
	};




	CModel();
	CModel(const CModel& aCopy);
	~CModel();

	CModel& operator=(const CModel& aModel);
	CModel& operator=(CModel&& aModel);

	//FBX
	bool Initialize(CEffect* aEffect, CSurface* aSurface, const CLoaderMesh* aLoadedMesh);
	bool Initialize(CEffect* aEffect, CSurface* aSurface, const CU::GrowingArray<CLoaderMesh*>& aLoadedMesh);

	bool Initialize(CEffect * aEffect, CSurface * aSurface, CU::GrowingArray<SVertexDataCube>& aVertexList, CU::GrowingArray<unsigned int>& aIndexList);
	bool Initialize(CEffect * aEffect, CSurface * aSurface, CU::GrowingArray<SVertexDataCube>& aVertexList);
	//Shape
	bool Initialize(CEffect* aEffect, CSurface* aSurface);

	void Render(SForwardRenderModelParams& aParamObj);
	void Render(SDeferredRenderModelParams& aParamObj);


	inline bool GetInitialized() const;
	inline const SSphereColData& GetCollisionData() const;

	void UpdateConstantBuffer(const eShaderStage aShaderStage, const void* aBufferStruct, const unsigned int aBufferSize);
	inline void AddConstantBuffer(const eShaderStage aShaderStage, ID3D11Buffer* aConstantBuffer);
	inline bool HasConstantBuffer(const eShaderStage aShaderStage) const;

	inline const CU::AABB& GetBoundingBox() const;
	inline bool IsAlphaModel() const;
	inline const struct aiScene* GetScene() const;
	inline void SetScene(const struct aiScene* aScene);

	inline bool HasAnimations();

	inline void AddRef();
	inline void RemoveRef();

	inline int GetRefCount();
	CU::Matrix44f GetBoneTransform(const float aTime, const char * aAnimationState, const char* aBoneName);

	inline bool HasBones() const;

	std::vector<mat4>& GetBones(float aTime, const char * aAnimationState, const bool aAnimationLooping);

	__forceinline const std::string& GetName() const;

private:

	SLodData& GetCurrentLODModel(const CU::Vector3f& aModelPosition);

	void InitConstBuffers();
	bool InitBuffers(CU::GrowingArray<SVertexDataCube>& aVertexList, CU::GrowingArray<unsigned int>& aIndexList);
	bool InitBuffers(CU::GrowingArray<SVertexDataCube>& aVertexList);
	bool InitBuffers(const CLoaderMesh * aLoadedMesh);
	void UpdateCBuffer(SForwardRenderModelParams& aParamObj);
	void UpdateCBuffer(SDeferredRenderModelParams& aParamObj);


	inline void SetBoundingBox(const CU::AABB& aAABB);

private:
	CU::StaticArray<ID3D11Buffer*, static_cast<int>(eShaderStage::eLength)> myConstantBuffers;

	std::map<std::string, CSceneAnimator> mySceneAnimators;
	CSceneAnimator* mySceneAnimator;
	CSceneAnimator* myBindposeSceneAnimator;

#ifdef _DEBUG
	std::string myFilePath;
#endif // _DEBUG
	CEffect* myDeferredEffect;
	CEffect* myForwardEffect;
	CSurface* mySurface;
	CDXFramework* myFramework;

	CU::GrowingArray<SLodData> myLODModels;

	ID3D11Buffer* myCbuffer; // vertexBuffer
	ID3D11Buffer* myLightBuffer; // pixelbuffer
	ID3D11Buffer* myPixelBuffer;
	ID3D11Buffer* myBoneBuffer;

	static const unsigned int ourMaxBoneBufferSize = 32u * sizeof(CU::Matrix44f);

	const struct aiScene* myScene;

	std::atomic_bool myIsInitialized;

	//mebe a radius is enough
	CU::AABB myAABB;


	unsigned int myVertexSize;

	SSphereColData mySphereColData;

	bool myIsAlphaModel;

	int myRefCount;

};
inline const SSphereColData& CModel::GetCollisionData() const
{
	return mySphereColData;
}

inline const CU::AABB & CModel::GetBoundingBox() const
{
	return myAABB;
}

inline void CModel::AddConstantBuffer(const eShaderStage aShaderStage, ID3D11Buffer* aConstantBuffer)
{
	myConstantBuffers[static_cast<int>(aShaderStage)] = aConstantBuffer;
}

inline bool CModel::HasConstantBuffer(const eShaderStage aShaderStage) const
{
	return myConstantBuffers[static_cast<int>(aShaderStage)] != nullptr;
}

inline void CModel::SetBoundingBox(const CU::AABB & aAABB)
{
	myAABB = aAABB;
}


inline const aiScene* CModel::GetScene() const
{
	return myScene;
}

inline void CModel::SetScene(const aiScene* aScene)
{
	myScene = aScene;
}

inline bool CModel::HasAnimations()
{
	return mySceneAnimator != nullptr;
}

inline void CModel::AddRef()
{
	myRefCount++;
}

inline void CModel::RemoveRef()
{
	myRefCount--;
}

inline int CModel::GetRefCount()
{
	return myRefCount;
}

inline bool CModel::HasBones() const
{
	return myBoneBuffer != nullptr;
}

inline bool CModel::IsAlphaModel() const
{
	return myIsAlphaModel;
}

inline bool CModel::GetInitialized() const
{
	return myIsInitialized;
}

const std::string& CModel::GetName() const
{
#ifdef _DEBUG
	return myFilePath;
#else // !_DEBUG
	return "";
#endif // _DEBUG
}
