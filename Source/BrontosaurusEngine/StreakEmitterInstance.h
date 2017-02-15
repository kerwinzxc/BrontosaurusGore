#pragma once
#include "InstanceID.h"
#include "Texture.h"
#include "EmitterData.h"
#include "../CommonUtilities/vector4.h"
#include "../CommonUtilities/vector3.h"
#include "../CommonUtilities/matrix44.h"
#include <climits>

namespace CU
{
	class Time;
	class Camera;
}


class CStreakEmitter;

class CStreakEmitterInstance
{
public:
	CStreakEmitterInstance();
	CStreakEmitterInstance(const SStreakEmitterData& aEmitterValues);

	CStreakEmitterInstance(const CStreakEmitterInstance& aStreakEmitterInstance) = delete;
	~CStreakEmitterInstance();

	void Update(const CU::Time & aDeltaTime);
	void Render();

	inline CU::Vector3f GetPosition();
	inline void SetPosition(const CU::Vector3f&);

	void Activate();
	void Deactivate();
	bool IsVisible() const;

	inline InstanceID GetInstanceID();
	inline void SetInstandeID(const InstanceID aID);

private:
	CU::Matrix44f myToWorldSpace;
	InstanceID myInstanceID;
	CStreakEmitter* myEmitter;

	bool myIsActivated;
};

inline CU::Vector3f CStreakEmitterInstance::GetPosition()
{
	return myToWorldSpace.GetPosition();
}

inline void CStreakEmitterInstance::SetPosition(const CU::Vector3f & aPosition)
{
	myToWorldSpace.SetPosition(aPosition);
}




inline InstanceID CStreakEmitterInstance::GetInstanceID()
{
	return myInstanceID;
}

inline void CStreakEmitterInstance::SetInstandeID(const InstanceID aID)
{
	myInstanceID = aID;
}
