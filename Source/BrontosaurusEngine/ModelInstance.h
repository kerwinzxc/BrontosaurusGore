#pragma once
#include "../BrontosaurusEngine/ModelShapes.h"
#include "../CommonUtilities/matrix44.h"
#include "../CommonUtilities/AABB.h"
#include "../CommonUtilities/VectorOnStack.h"

#include <map>

namespace CU
{
	template <typename ObjectType, typename SizeType = unsigned int, bool USE_SAFE_MODE = true>
	class GrowingArray;
}

namespace Lights
{
	struct SDirectionalLight;
}

class CModel;
class CRenderCamera;
class CPointLightInstance;
class CSceneAnimator;

class CModelInstance
{
public:
	typedef int ModelId;

	friend class CModelManager;
	friend class CModelComponent;

public:
	//CModelInstance();
	~CModelInstance();

	CModelInstance(const char* aModelPath);
	CModelInstance(const char* aModelPath, const CU::Matrix44f& aTransformation);

	CModelInstance(const SShape aShape);
	CModelInstance(const SShape aShape, const CU::Matrix44f& aTransformation);

	CModelInstance(ModelId aModel, const CU::Matrix44f& aTransformation);

public:
	bool ShouldRender();
	inline const CU::Matrix44f& GetTransformation() const;
	inline const CU::Matrix44f& GetLastFrameTransformation() const;
	void SetVisibility(const bool aFlag);
	inline bool GetVisibility() { return myIsVisible; }

	void Render(Lights::SDirectionalLight* aLight, CU::VectorOnStack<CPointLightInstance, 8>& aPointLightList);
	void Render();
	//Edvin testar med kamera
	void Render(Lights::SDirectionalLight* aLight, CU::VectorOnStack<CPointLightInstance, 8>& aPointLightList, CRenderCamera& aRenderToCamera);

	void Update(const CU::Time aDeltaTime);

	void SetTransformation(CU::Matrix44f& aTransformation);
	void SetPosition(CU::Vector3f aPosition);
	void Rotate(float aRotation, CU::Axees aAxis) { myTransformation.Rotate(aRotation, aAxis); }

	void ChangeAnimation(const char* aAnimationKey);
	void SetAnimationLooping(const bool aValue);
	void ResetAnimation();

	void SetHighlightIntencity(const float aHighlightIntencity);
	
	inline ModelId GetModelID();

	CU::AABB GetModelBoundingBox();

	inline const char* GetAnimationState() { return myCurrentAnimation; }
	inline float GetAnimationCounter() { return myAnimationCounter; }

private:
	CU::Matrix44f myTransformation;
	CU::Matrix44f myLastFrame;

	ModelId myModel;

	const char* myCurrentAnimation;

	float myAnimationCounter;
	float myHighlightIntencity;
	bool myIsVisible;
	bool myHasAnimations;
	bool myAnimationLooping;
};

inline const CU::Matrix44f& CModelInstance::GetTransformation() const
{
	return myTransformation;
}

inline const CU::Matrix44f & CModelInstance::GetLastFrameTransformation() const
{
	return myLastFrame;
}

inline void CModelInstance::SetVisibility(const bool aFlag)
{
	myIsVisible = aFlag;
}


inline int CModelInstance::GetModelID()
{
	return myModel;
}


