#pragma once
#include "EmitterData.h"
#include "Texture.h"
#include "../CommonUtilities/vector4.h"
#include "../CommonUtilities/vector3.h"
#include "../CommonUtilities/matrix44.h"
#include <climits>

namespace CU
{
	class Time;
	class Camera;
}

class CEffect;
class CDXFramework;
struct ID3D11Buffer;



class CStreakEmitter
{
public:
	struct SStreak
	{
		CU::Vector4f pos;
		float size;
		float alpha; //size 6
		float trash1;
		float trash2;
	};

	//doesn't need to be alligned since it's not going to the GPU ?
	struct SStreakLogic
	{
		float lifeTime;
		float lifeLeft;
	};

	struct SCameraPosition
	{
		CU::Vector4f position;
	};

public:
	CStreakEmitter(const SStreakEmitterData & aEmitterData);
	CStreakEmitter(const CStreakEmitter& aStreakEmitterInstance) = delete;
	~CStreakEmitter();


	void Init();

	void Update(const CU::Time & aDeltaTime, const CU::Matrix44f& aToWorld);
	void Render(const CU::Matrix44f & aToWorld);


	inline void Activate();
	inline void Deactivate();
	inline bool IsVisible() const;

private:
	void EmitParticle(const CU::Matrix44f& aToWorld);
	void UpdateCBuffers(const CU::Matrix44f& aToWorldSpace);
	bool InitBuffers();


private:
	float myEmitDelta;
	float myEmitTimer;

	SStreakEmitterData myEmitterData;

	CU::GrowingArray<SStreak> myParticles;
	CU::GrowingArray<SStreakLogic> myParticleLogic;

	CTexture* myTexture;

	CEffect* myEffect;
	CDXFramework* myFramework;

	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myGeometryCBuffer;
	ID3D11Buffer* myVertexCBuffer;

	bool myIsActivated;
	static constexpr unsigned short ourMaxNrOfParticles = /*USHRT_MAX*/512u;
};

void CStreakEmitter::Activate()
{
	myIsActivated = true;
}

void CStreakEmitter::Deactivate()
{
	myIsActivated = false;
}

bool CStreakEmitter::IsVisible() const
{
	return myParticles.Size() > 0;
}

