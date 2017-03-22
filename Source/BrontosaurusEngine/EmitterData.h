#pragma once
#include "../CommonUtilities/StaticArray.h"
#include <Vector3.h>
#include <Vector4.h>

#ifndef STATIC_SIZEOF
#define STATIC_SIZEOF(x) {char STATIC_SIZEOF_TEMP[(x)]; STATIC_SIZEOF_TEMP = 1;}
#endif // !STATIC_SIZEOF

enum class eLerpCurve : char
{
	eLinear,
	eExponential,
	eEaseOut,
	eEaseIn,
	eSmoothStep,
	eSmootherStep,
};


struct SEmitterKeyframe
{
	SEmitterKeyframe()
	{
		time = -1.0f; // nullo
	}

	SEmitterKeyframe(float t, const CU::Vector4f& v)
	{
		time = t; 
		value = v;
	}

	//bool operator==(const SEmitterKeyframe& aLeft)
	//{
	//	return aLeft.Time == this->Time & aLeft.Value == this->Value;
	//}


	float time;

	CU::Vector4f value;

};

struct SEmitterData
{
	SEmitterData()
	{
		//STATIC_SIZEOF(alignof(SEmitterData));
		//STATIC_SIZEOF(sizeof(*this));
	}

	std::string TexturePath;


	CU::Vector4f StartColor;
	CU::Vector4f EndColor;

	CU::Vector3f MinEmissionVelocity;
	CU::Vector3f MaxEmissionVelocity;
	
	CU::Vector3f MinEmissionArea;
	CU::Vector3f MaxEmissionArea;

	CU::Vector3f Gravity;
	
	float StartSize = 1.f;
	float EndSize = 1.f;
	
	float EmissionRate = 10.f;

	float StartRotation = 0.f;
	float EndRotation = 0.f;

	float MinParticleLifeTime = 1.f;
	float MaxParticleLifeTime = 1.f;
	
	CU::StaticArray<SEmitterKeyframe, 2> ColorOverLife; // sort these badboys


	int NumOfParticles = 1000;


	eLerpCurve RotationCurve = eLerpCurve::eLinear;
	eLerpCurve ColorCurve = eLerpCurve::eLinear;
	eLerpCurve SizeCurve = eLerpCurve::eLinear;
	bool UseGravity = false;
	bool ShouldLoop = true;
	float Lifetime = 0.f;
};

struct  SStreakEmitterData
{
	const char* TexturePath;
	float EmissionRate;

	float ParticleLifetime;

	float StartSize;
	float EndSize;

	float StartAlpha;
	float EndAlpha;

	float EmissonLifeTime;

	int NumOfParticles;
};


struct SParticle
{
	CU::Vector4f position; // stores rotation in W
	float size;
	CU::Vector4f color;
};

//doesn't need to be alligned since it's not going to the GPU ?
struct SParticleLogic
{
	CU::Vector3f movementDir;
	float lifeTime;
	float lifetimeLeft;
	float rotation;
};
