#pragma once
#include "vector4.h"
#include "Texture.h"

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
	CParticleEmitter();
	CParticleEmitter(const CParticleEmitter& aParticleEmitter);
	~CParticleEmitter();
	void Init(const SEmitterData& EmitterData);
	void Render(const CU::Matrix44f & aToWorldSpace, const CU::GrowingArray<SParticle, unsigned short, false>& aParticleList);
	void Destroy();

	CParticleEmitter& operator=(const CParticleEmitter& aParticleEmitter);
	CParticleEmitter& operator=(CParticleEmitter&& aParticleEmitter);

private:
	void ResizeVertexBuffer(const CU::GrowingArray<SParticle, unsigned short, false>& aParticleList);
	void UpdateCBuffers(const CU::Matrix44f& aToWorldSpace);
	bool InitBuffers();

private:
	unsigned short myMaxNrOfParticles;

	CTexture*		myTexture;
	CEffect*		myEffect;

	CDXFramework*	myFramework;
	ID3D11Buffer*	myVertexBuffer;
	ID3D11Buffer*	myModelBuffer;

};