#pragma once
#include "Component.h"
#include <matrix44.h>

namespace CU
{
	template <typename TYPE>
	class Matrix44;
	using Matrix44f = Matrix44<float>;

	class Camera;
}

class CCameraComponent : public CComponent
{
public:
	//friend class CCameraComponentManager;
	CCameraComponent();
	~CCameraComponent();
	
	void InitOffsetPosition();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)override;

	inline void SetCamera(CU::Camera& aCamera);

private:
	void Pitch(const float aPitch);

	CU::Vector3f myOffsetPosition;
	CU::Camera* myCamera;
	bool myUnlocked;
};

inline void CCameraComponent::SetCamera(CU::Camera& aCamera)
{
	myCamera = &aCamera;
}
