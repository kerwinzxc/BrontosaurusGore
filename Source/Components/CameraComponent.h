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
	friend class CCameraComponentManager;
	
	void InitOffsetPosition();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)override;
	void Destroy();

	inline void SetCamera(CU::Camera& aCamera);

private:
	CCameraComponent();
	~CCameraComponent();

	CU::Vector3f myOffsetPosition;
	CU::Camera* myCamera;
	bool myUnlocked;
};

inline void CCameraComponent::SetCamera(CU::Camera& aCamera)
{
	myCamera = &aCamera;
}
