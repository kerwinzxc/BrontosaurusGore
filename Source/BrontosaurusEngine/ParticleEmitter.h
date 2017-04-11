#pragma once
#include <mutex>
#include "Texture.h"
#include "../CommonUtilities/GrowingArray.h"
#include "../CommonUtilities/StaticArray.h"
#include "../CommonUtilities/vector3.h"
#include "../CommonUtilities/vector4.h"
#include "binary_tree.h"
#include "EmitterData.h"
#include "../CommonUtilities/matrix44.h"
#include "../LuaWrapper/file_watcher.h"


namespace Particles
{
	class IParticleSpawner;
	class IParticleUpdater;
}

namespace CU
{
	class CJsonValue;
	class Time;
	class Camera;

	template<typename TYPE>
	class Matrix44;
	using Matrix44f = Matrix44<float>;
}

class CEffect;
class CDXFramework;
class CParticleEmitterInstance;
struct ID3D11Buffer;
struct SParticle;
struct SEmitterData;

class CParticleEmitter
{
public:
	enum class RenderMode
	{
		eMetaBall,
		eBillboard,
		eNURBSSphere,
		eSize,
	};
	enum class EmitterType
	{
		eSphere,
		ePoint,
		eNone,
	};
	enum class SpreadType
	{
		eSphere,
		eNone
	};
	enum class LifetimeType
	{
		eConstant,
		eRandom,
	};

	CParticleEmitter();

	explicit CParticleEmitter(const CU::CJsonValue& aJsonValue);
	CParticleEmitter(const CParticleEmitter& aParticleEmitter) = delete;
	CParticleEmitter(const CParticleEmitter&& aParticleEmitter) = delete;
	CParticleEmitter& operator=(const CParticleEmitter& aParticleEmitter) = delete;
	CParticleEmitter& operator=(CParticleEmitter&& aParticleEmitter) = delete;

	~CParticleEmitter();

	void UpdateInstance(const CU::Time& aTime, CParticleEmitterInstance& aInstance);

	void Render(const CU::Matrix44f & aToWorldSpace, const CU::GrowingArray<SParticle, unsigned int, false>& aParticleList, RenderMode aRenderMode);
	void Destroy();

	bool HasEffects() const;
	
	void AddRef();
	void RemoveRef();
	float GetLifetime() const;
	float GetEmitTime();
	unsigned GetMaxParticles();
	CParticleEmitter::RenderMode GetRenderMode() const;
	

private:
	void Init(const CU::CJsonValue& aJsonValue);

	void InitialVelocity(SParticleLogic& aParticleLogic);
	void SpawnParticle(SParticle& aParticle, SParticleLogic& aParticleLogic);
	void UpdateParticles(SParticle& aParticle, SParticleLogic& aLogic, const float aDt);

	void ParseSpawnParameters(const CU::CJsonValue& aJsonValue);
	void ParseUpdateParameters(const CU::CJsonValue& aJsonValue);
	void ParseParticle(const CU::CJsonValue& aJsonValue);
	EmitterType ParseEmitterType(const std::string& aTypeString);
	SpreadType GetSpreadType(const std::string& aTypeString);
	void ParseEmissionArea(const CU::CJsonValue& aJsonValue);
	RenderMode GetRenderMode(const std::string& aRenderModeString);

	void ParseRender(const CU::CJsonValue& aJsonValue);

	ID3D11Buffer* GetVertexBuffer();
	void ResizeVertexBuffer(const CU::GrowingArray<SParticle, unsigned int, false>& aParticleList);
	void UpdateCBuffers(const CU::Matrix44f& aToWorldSpace);
	bool InitBuffers();

private:
	union Value
	{
		struct MimMax
		{
			CU::Vector3f min;
			CU::Vector3f max;
		};
		float radius;
		float angle;
	};

	struct Spread
	{
		SpreadType type;
		Value value;
	};

	struct Emitter
	{
		EmitterType type;
		unsigned int maxNrOfParticles = 0;
		unsigned int emissionRate = 0;

		bool loop = false;
		float lifetime = 0.f;
		
		Spread spread;
		
		Value value;
	};

	struct RenderStruct
	{
		RenderMode renderMode = RenderMode::eMetaBall;
		CTexture* myTexture = nullptr;
		CU::Vector4f color;
	};
	struct Lifetime
	{
		LifetimeType type;
		float min;
		float max;
	};
	struct ParticleData
	{
		Lifetime lifetime;
		CU::GrowingArray<Particles::IParticleSpawner*> spawners;
		CU::GrowingArray<Particles::IParticleUpdater*> updaters;
	};
	struct EmitterData
	{
		
		std::string name;
		int id = 0;
		RenderStruct render;
		Emitter emitter;

		ParticleData particles;

	} myEmitterData;
	

	CU::StaticArray<CEffect*, static_cast<int>(RenderMode::eSize)> myRenderEffects;

	CDXFramework*	myFramework;
	ID3D11Buffer*	myVertexBuffer;
	ID3D11Buffer*	myModelBuffer;

	std::mutex myUpdateVBufferMutex;
	unsigned int myRefCount;
	CU::Matrix44f myCurrentInstaceTransform;
	

#ifndef _RETAIL_BUILD
	std::mutex myUpdateMutex;
	std::mutex myRenderMutex;
#endif
};