#pragma once
#include "../CommonUtilities/matrix44.h"
#include "../CommonUtilities/VectorOnStack.h"
#include "../CommonUtilities/Sphere.h"
#include <map>

enum class eAnimationState;

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
	CModelInstance(const std::string& aModelPath);
	CModelInstance(const CModelInstance& aCopy) = delete;
	~CModelInstance();

	bool ShouldRender();
	CU::Sphere GetModelBoundingSphere();

	void RenderDeferred();
	void RenderDeferred(CRenderCamera & aRenderToCamera);
	void RenderForward(Lights::SDirectionalLight* aLight, CU::VectorOnStack<CPointLightInstance, 8>& aPointLightList);
	void RenderForward(Lights::SDirectionalLight* aLight, CU::VectorOnStack<CPointLightInstance, 8>& aPointLightList, CRenderCamera& aRenderToCamera);

	void Update(const CU::Time aDeltaTime);

	void SetTransformation(const CU::Matrix44f& aTransformation);
	void SetPosition(CU::Vector3f aPosition);
	void Rotate(float aRotation, CU::Axees aAxis) { myTransformation.Rotate(aRotation, aAxis); }

	float GetAnimationDuration(const eAnimationState aAnimationState) const;
	void SetAnimation(const eAnimationState aAnimationKey);
	void SetAnimationLerpie(const float aLerpValue);
	void SetNextAnimation(const eAnimationState aAnimationKey);
	void SetAnimationLooping(const bool aValue);
	void ResetAnimation();
	eAnimationState GetAnimationState() const;
	float GetAnimationCounter() const;
	bool GetAnimationStates(CU::GrowingArray<eAnimationState>& aAnimationStatesOut) const;
	const std::string& GetFilePath() const;

	void SetHighlightIntencity(const float aHighlightIntencity);
	void SetHighlight(const CU::Vector4f& aColor, float anIntensivit);

	inline const CU::Matrix44f& GetTransformation() const;
	inline const CU::Matrix44f& GetLastFrameTransformation() const;
	inline void SetVisibility(const bool aIsVisible);
	inline bool GetVisibility();
	inline ModelId GetModelID();
	inline void SetIgnoreDepth(const bool aShouldIgnore);
	inline bool GetIgnoreDepth() const;

private:
	CU::Matrix44f myTransformation;
	CU::Matrix44f myLastFrame;

	CU::Vector4f myHighlightColor;

	eAnimationState myCurrentAnimation;
	eAnimationState myNextAnimation;

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

inline void CModelInstance::SetVisibility(const bool aIsVisible)
{
	myIsVisible = aIsVisible;
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

inline bool CModelInstance::GetVisibility()
{
	return myIsVisible;
}
