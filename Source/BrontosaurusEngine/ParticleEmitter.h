#pragma once
#include "vector4.h"
#include "Texture.h"
#include <mutex>

namespace CU
{
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

	CParticleEmitter();
	CParticleEmitter(const CParticleEmitter& aParticleEmitter);
	~CParticleEmitter();
	void Init(const SEmitterData& EmitterData);
	void Render(const CU::Matrix44f & aToWorldSpace, const CU::GrowingArray<SParticle, unsigned int, false>& aParticleList, RenderMode aRenderMode);
	void Destroy();

	bool HasEffects() const;
	CParticleEmitter& operator=(const CParticleEmitter& aParticleEmitter);
	CParticleEmitter& operator=(CParticleEmitter&& aParticleEmitter);

private:

	ID3D11Buffer* GetVertexBuffer();
	void ResizeVertexBuffer(const CU::GrowingArray<SParticle, unsigned int, false>& aParticleList);
	void UpdateCBuffers(const CU::Matrix44f& aToWorldSpace);
	bool InitBuffers();

private:
	CU::StaticArray<CEffect*, static_cast<int>(RenderMode::eSize)> myRenderEffects;

	CTexture*		myTexture;

	CDXFramework*	myFramework;
	ID3D11Buffer*	myVertexBuffer;
	ID3D11Buffer*	myModelBuffer;

	unsigned int myMaxNrOfParticles;
	std::mutex myUpdateVBufferMutex;
};