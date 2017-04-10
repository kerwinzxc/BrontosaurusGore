#include "stdafx.h"
#include "ParticleEmitterInstance.h"
#include "Renderer.h"
#include "Engine.h"
#include "ParticleEmitter.h"

#include "ParticleEmitterManager.h"

#include "../CommonUtilities/CUTime.h"


//#define STATIC_SIZEOF(x) {char STATIC_SIZEOF_TEMP[(x)]; STATIC_SIZEOF_TEMP = 1;}


namespace
{
#ifndef RAND_FLOAT_RANGE
#define RAND_FLOAT_RANGE(LOW, HIGH) (LOW) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((HIGH)-(LOW))));
#endif

	float fLerp(float aStart, float aEnd, float aTime)
	{
		return (aStart + aTime*(aEnd - aStart));
		//return (aEnd + aTime*(aStart - aEnd));
	}

	float CalcCurve(float t, eLerpCurve aCurveType)
	{
		const float pi = 3.1415f;

		switch (aCurveType)
		{
		case eLerpCurve::eLinear:
			return t;
		case eLerpCurve::eExponential:
			return t * t;
		case eLerpCurve::eEaseOut:
			return std::sin(t * pi * 0.5f);
		case eLerpCurve::eEaseIn:
			return 1.f - std::cos(t * pi * 0.5f);
		case eLerpCurve::eSmoothStep:
			return t*t * (3.f - 2.f*t);
		case eLerpCurve::eSmootherStep:
			return t*t*t * (t * (6.f*t - 15.f) + 10.f);
		default:
			return 0.0f;
			break;
		}
	}

	CU::Vector4f vectorFlerp(const CU::Vector4f& aStart, const CU::Vector4f& aEnd, float aTime)
	{

		CU::Vector4f ret;

		ret.x = fLerp(aStart.x, aEnd.x, aTime);
		ret.y = fLerp(aStart.y, aEnd.y, aTime);
		ret.z = fLerp(aStart.z, aEnd.z, aTime);
		ret.w = fLerp(aStart.w, aEnd.w, aTime);

		return ret;
	}

	float Distance2(const CU::Vector4f& p1, const CU::Vector4f& p2)
	{
		return ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z));
	}
}

int CParticleEmitterInstance::ourIds = 0;

CParticleEmitterInstance::CParticleEmitterInstance(int anId)
{
	myEmitterID = anId;
	myInstanceID = ourIds++;

	Init();

	ResetLifetime();
	ResetSpawnTimer();
}

CParticleEmitterInstance::~CParticleEmitterInstance()
{
	CEngine* engine = CEngine::GetInstance();
	assert(engine != nullptr && "CEngine was nullptr");
	engine->GetParticleEmitterManager().RemoveParticleEmitter(myEmitterID);
}


void CParticleEmitterInstance::Update(const CU::Time& aDeltaTime)
{
	//float deltaTime = aDeltaTime.GetSeconds();

	//myLifetime -= deltaTime;

	//if(myLifetime < 0)
	//{
	//	if(myEmitterData.ShouldLoop == true)
	//	{
	//		myLifetime = myEmitterData.Lifetime;
	//	}
	//	else
	//	{
	//		myIsActive = false;
	//	}
	//}

	//if (myIsActive == true)
	//{
	//	myEmitTimer += deltaTime;
	//	while (myEmitTimer >= myEmitDelta)
	//	{
	//		myEmitTimer -= myEmitDelta;
	//		EmitParticle();
	//	}
	//}
	//if (myParticles.Size() != myParticleLogic.Size())
	//{
	//	assert(false && "Particlelist and logiclist missalligned.");
	//}

	//for (unsigned short i = 0; i < myParticles.Size(); ++i)
	//{
	//	myParticleLogic[i].lifetimeLeft -= deltaTime;
	//	if (myParticleLogic[i].lifetimeLeft <= 0.0f)
	//	{
	//		myParticles.RemoveCyclicAtIndex(i);
	//		myParticleLogic.RemoveCyclicAtIndex(i);
	//		--i;
	//		continue;
	//	}

	//	if (myEmitterData.UseGravity == true)
	//	{
	//		myParticleLogic[i].movementDir = myParticleLogic[i].movementDir + (myEmitterData.Gravity * deltaTime);
	//	}


	//	CU::Vector3f deltaMovement = myParticleLogic[i].movementDir * deltaTime;
	//	float particleLifetime = 1.0f - (myParticleLogic[i].lifetimeLeft / myParticleLogic[i].lifeTime);
	//	


	//	myParticles[i].position += deltaMovement;


	//	
	//	myParticles[i].position.w = myParticleLogic[i].rotation + fLerp(
	//		myEmitterData.StartRotation, 
	//		myEmitterData.EndRotation, 
	//		CalcCurve(particleLifetime, myEmitterData.RotationCurve));

	//	myParticles[i].position.w *= 3.1415f / 180.f;


	//	myParticles[i].size = fLerp(
	//		myEmitterData.StartSize, 
	//		myEmitterData.EndSize, 
	//		CalcCurve(particleLifetime, myEmitterData.SizeCurve));
	//	
	//	
	//	
	//	SEmitterKeyframe startK = {0.0f, myEmitterData.StartColor};
	//	SEmitterKeyframe endK = {1.0f, myEmitterData.EndColor};


	//	for (int j = 0; j < myEmitterData.ColorOverLife.Size(); ++j)
	//	{
	//		if (myEmitterData.ColorOverLife[j].time < 0.0f)
	//			continue;

	//		if (myEmitterData.ColorOverLife[j].time < particleLifetime)
	//		{
	//			if (myEmitterData.ColorOverLife[j].time > startK.time)
	//			{
	//				startK = myEmitterData.ColorOverLife[j];
	//			}
	//		}


	//		if (myEmitterData.ColorOverLife[j].time > particleLifetime)
	//		{
	//			if (myEmitterData.ColorOverLife[j].time < endK.time)
	//			{
	//				endK = myEmitterData.ColorOverLife[j];
	//			}
	//		}

	//	}


	//	// start is bigger then life

	//	float colorT = (particleLifetime - startK.time) / (endK.time - startK.time);

	//	const float curve = CalcCurve(colorT, myEmitterData.ColorCurve);
	//	CU::Vector4f colr = vectorFlerp(
	//		startK.value,
	//		endK.value,
	//		curve);

	//	myParticles[i].color = colr;

	//}
}

void CParticleEmitterInstance::Render(CRenderCamera& aRenderCamera)
{
	if (myParticles.Size() > 0)
	{
		SRenderParticlesMessage msg;
		msg.particleEmitter = myEmitterID;
		msg.toWorld = myToWorldSpace;
		msg.particleList = myParticles;
		msg.myType = SRenderMessage::eRenderMessageType::eRenderParticles;
		DistanceSort(msg.particleList, aRenderCamera.GetCamera());
		aRenderCamera.AddRenderMessage(new SRenderParticlesMessage(msg));
	}
}

int CParticleEmitterInstance::GetEmitterId()
{
	return myEmitterID;
}

void CParticleEmitterInstance::ResetLifetime()
{
	myLifetime = CParticleEmitterManager::GetInstance().GetEmitter(GetEmitterId())->GetLifetime();
}

void CParticleEmitterInstance::ResetSpawnTimer()
{
	myEmitTimer = CParticleEmitterManager::GetInstance().GetEmitter(GetEmitterId())->GetEmitTime();
}

bool CParticleEmitterInstance::IsActive()
{
	return myIsActive;
}

void CParticleEmitterInstance::Init()
{
	SetVisibility(true);
	myParticles.Init(CParticleEmitterManager::GetInstance().GetEmitter(GetEmitterId())->GetMaxParticles());
	myParticleLogic.Init(CParticleEmitterManager::GetInstance().GetEmitter(GetEmitterId())->GetMaxParticles());
}

void CParticleEmitterInstance::EmitParticle()
{
	/*if (myParticles.Size() < static_cast<unsigned int>(myEmitterData.NumOfParticles))
	{

		SParticle particle;
		CU::Vector3f volumePos;
		volumePos.x = RAND_FLOAT_RANGE(myEmitterData.MinEmissionArea.x, myEmitterData.MaxEmissionArea.x);
		volumePos.y = RAND_FLOAT_RANGE(myEmitterData.MinEmissionArea.y, myEmitterData.MaxEmissionArea.y);
		volumePos.z = RAND_FLOAT_RANGE(myEmitterData.MinEmissionArea.z, myEmitterData.MaxEmissionArea.z);
		particle.position = volumePos * myToWorldSpace;
		particle.position.w = 1;


		particle.color = myEmitterData.StartColor;
		particle.size = myEmitterData.StartSize;

		SParticleLogic logic;
		logic.lifeTime = RAND_FLOAT_RANGE(myEmitterData.MinParticleLifeTime, myEmitterData.MaxParticleLifeTime);
		logic.lifetimeLeft = logic.lifeTime;
		logic.rotation = RAND_FLOAT_RANGE(0.0f, 360.0f);

		float x = RAND_FLOAT_RANGE(myEmitterData.MinEmissionVelocity.x, myEmitterData.MaxEmissionVelocity.x);
		float y = RAND_FLOAT_RANGE(myEmitterData.MinEmissionVelocity.y, myEmitterData.MaxEmissionVelocity.y);
		float z = RAND_FLOAT_RANGE(myEmitterData.MinEmissionVelocity.z, myEmitterData.MaxEmissionVelocity.z);

		logic.movementDir = CU::Vector3f(x, y, z);

		myParticles.Add(particle);
		myParticleLogic.Add(logic);
	}*/
}

void CParticleEmitterInstance::Activate()
{
	ResetLifetime();
	ResetSpawnTimer();
	myIsActive = true;
}

void CParticleEmitterInstance::Deactivate()
{
	myIsActive = false;
}

void CParticleEmitterInstance::DistanceSort(CU::GrowingArray<SParticle, unsigned int, false>& aParticleList, const CU::Camera& aCamera)
{
	CU::Vector4f cameraPosition(aCamera.GetPosition());

	auto compareLambda = [cameraPosition](const SParticle& aInput, const SParticle& aSecondInput)
	{
		float x = Distance2(aInput.position, cameraPosition);
		float y = Distance2(aSecondInput.position, cameraPosition);

		return (x < y);
	};

	aParticleList.QuickSort(compareLambda);
}


