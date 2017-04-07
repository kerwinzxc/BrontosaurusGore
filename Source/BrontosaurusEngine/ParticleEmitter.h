#pragma once
#include "Texture.h"
#include "../CommonUtilities/GrowingArray.h"
#include "../CommonUtilities/StaticArray.h"
#include <mutex>

namespace Particles
{
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

	CParticleEmitter();

	explicit CParticleEmitter(const CU::CJsonValue& aJsonValue);
	CParticleEmitter(const CParticleEmitter& aParticleEmitter);
	~CParticleEmitter();
	void Init(const SEmitterData& EmitterData);
	void Render(const CU::Matrix44f & aToWorldSpace, const CU::GrowingArray<SParticle, unsigned int, false>& aParticleList, RenderMode aRenderMode);
	void Destroy();

	bool HasEffects() const;
	CParticleEmitter& operator=(const CParticleEmitter& aParticleEmitter);
	CParticleEmitter& operator=(CParticleEmitter&& aParticleEmitter) noexcept;
	void AddRef();
	void RemoveRef();
private:


	void ParseColor(const CU::CJsonValue& aJsonValue);
	void ParseSize(const CU::CJsonValue& aJsonValue);
	void ParseLifetime(const CU::CJsonValue& aJsonValue);
	void ParseVelocity(const CU::CJsonValue& aJsonValue);
	void ParseParticle(const CU::CJsonValue& aJsonValue);
	EmitterType ParseEmitterType(const std::string& aTypeString);
	void ParseEmissionArea(const CU::CJsonValue& aJsonValue);
	RenderMode GetRenderMode(const std::string& aRenderModeString);

	void ParseRender(const CU::CJsonValue& aJsonValue);

	ID3D11Buffer* GetVertexBuffer();
	void ResizeVertexBuffer(const CU::GrowingArray<SParticle, unsigned int, false>& aParticleList);
	void UpdateCBuffers(const CU::Matrix44f& aToWorldSpace);
	bool InitBuffers();

private:
	struct EmitterData
	{
		std::string name;
		int id = 0;
		unsigned int maxNrOfParticles = 0;
		unsigned int emissionRate = 0;
		bool loop = false;
		float lifetime = 0.f;
		struct
		{
			RenderMode renderMode = RenderMode::eMetaBall;
			CTexture* myTexture = nullptr;
		} render;

		CU::GrowingArray<Particles::IParticleUpdater*> updaters;
		
	} myEmitterData;

	CU::StaticArray<CEffect*, static_cast<int>(RenderMode::eSize)> myRenderEffects;

	CDXFramework*	myFramework;
	ID3D11Buffer*	myVertexBuffer;
	ID3D11Buffer*	myModelBuffer;

	std::mutex myUpdateVBufferMutex;
	unsigned int myRefCount;
	
};