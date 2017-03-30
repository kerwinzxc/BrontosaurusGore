#pragma once
#include "../CommonUtilities/matrix44.h"
#include "../CommonUtilities/VectorOnStack.h"
#include "../CommonUtilities/Sphere.h"
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
	CModelInstance(const char* aModelPath);
	~CModelInstance();

	bool ShouldRender();
	CU::Sphere GetModelBoundingSphere();
	inline const CU::Matrix44f& GetTransformation() const;
	inline const CU::Matrix44f& GetLastFrameTransformation() const;
	void SetVisibility(const bool aFlag);
	inline bool GetVisibility() { return myIsVisible; }

	void RenderDeferred();
	void RenderDeferred(CRenderCamera & aRenderToCamera);
	void RenderForward(Lights::SDirectionalLight* aLight, CU::VectorOnStack<CPointLightInstance, 8>& aPointLightList);
	void RenderForward(Lights::SDirectionalLight* aLight, CU::VectorOnStack<CPointLightInstance, 8>& aPointLightList, CRenderCamera& aRenderToCamera);

	void Update(const CU::Time aDeltaTime);

	void SetTransformation(const CU::Matrix44f& aTransformation);
	void SetPosition(CU::Vector3f aPosition);
	void Rotate(float aRotation, CU::Axees aAxis) { myTransformation.Rotate(aRotation, aAxis); }

	void SetAnimation(const std::string& aAnimationKey);
	void SetAnimationLerpie(const float aLerpValue);
	void SetNextAnimation(const std::string& aAnimationKey);
	void SetAnimationLooping(const bool aValue);
	void ResetAnimation();
	const std::string& GetAnimationState() const;
	float GetAnimationCounter() const;

	
	inline ModelId GetModelID();
	void SetHighlightIntencity(const float aHighlightIntencity);

	void SetHighlight(const CU::Vector4f& aColor, float anIntensivit);

	inline void SetIgnoreDepth(const bool aShouldIgnore);
	inline bool GetIgnoreDepth() const;

private:

	CU::Matrix44f myTransformation;
	CU::Matrix44f myLastFrame;

	CU::Vector4f myHighlightColor;

	std::string myCurrentAnimation;
	std::string myNextAnimation;

	ModelId myModel;
	
	float myAnimationLerpie;
	float myAnimationCounter;
	float myHighlightIntencity;
	
	bool myIsVisible;
	bool myHasAnimations;
	bool myAnimationLooping;
	bool myIgnoreDepth;
};

inline const CU::Matrix44f& CModelInstance::GetTransformation() const
{
	return myTransformation;
}

inline const CU::Matrix44f& CModelInstance::GetLastFrameTransformation() const
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


inline void CModelInstance::SetIgnoreDepth(const bool aShouldIgnore)
{
	myIgnoreDepth = aShouldIgnore;
}

inline bool CModelInstance::GetIgnoreDepth() const
{
	return myIgnoreDepth;
}
