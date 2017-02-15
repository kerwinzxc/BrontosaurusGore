#pragma once
#include "../CommonUtilities/matrix44.h"

using FireEmitterID = int;

struct SFireEmitterData;

class CFireEmitterInstance
{
public:
	CFireEmitterInstance();
	CFireEmitterInstance(const CFireEmitterInstance& aOther);
	CFireEmitterInstance(CFireEmitterInstance&& aTemporary);
	~CFireEmitterInstance();

	CFireEmitterInstance& operator=(const CFireEmitterInstance& aOther);
	CFireEmitterInstance& operator=(CFireEmitterInstance&& aTemporary);

	void Init(const SFireEmitterData& aFireEmitterData);
	void Render();

	inline CU::Matrix44f& GetTransformation();
	inline void SetPosition(const CU::Vector3f& aPosition);
	inline const CU::Vector3f& GetPosition() const;

private:
	CU::Matrix44f myTransformation;
	FireEmitterID myFireEmitter;
};

inline CU::Matrix44f& CFireEmitterInstance::GetTransformation()
{
	return myTransformation;
}

inline void CFireEmitterInstance::SetPosition(const CU::Vector3f& aPosition)
{
	myTransformation.SetPosition(aPosition);
}

inline const CU::Vector3f& CFireEmitterInstance::GetPosition() const
{
	return myTransformation.myPosition;
}
