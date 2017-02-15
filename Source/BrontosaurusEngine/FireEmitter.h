#pragma once

#include "FireEmitterData.h"

class CEffect;
class CTexture;
struct ID3D11Buffer;

class CFireEmitter
{
public:
	CFireEmitter();
	CFireEmitter(const CFireEmitter& aCopy);
	CFireEmitter(CFireEmitter&& aTemporary);
	~CFireEmitter();

	CFireEmitter& operator=(const CFireEmitter& aCopy);
	CFireEmitter& operator=(CFireEmitter&& aTemporary);

	void Init(const SFireEmitterData& aData);
	void Destroy();

	void Render(const CU::Time aFrameTime, const CU::Matrix44f& aToWorldMatrix);

	inline int AddRef();
	inline int DecRef();
	inline bool IsInitialized() const;

private:
	bool InitInputBuffers();
	bool InitConstantBuffers();
	bool InitTextures(const SFireEmitterData& aData);
	void UpdateBuffers(const CU::Time aFrameTime, const CU::Matrix44f& aToWorldMatrix);
	void SetTextures();

	SFireEmitterData myFireEmitterData;

	CEffect* myEffect;

	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myIndexBuffer;

	ID3D11Buffer* myMatrixBuffer;
	ID3D11Buffer* myNoiseBuffer;
	ID3D11Buffer* myDistortionBuffer;

	CTexture* myFireTexture;
	CTexture* myNoiseTexture;
	CTexture* myAlphaTexture;

	int myRefCount;

	static const unsigned int ourVertexSize = sizeof(float) * 6u;
	static const unsigned int ourIndexCount = 6u;
};

inline int CFireEmitter::AddRef()
{
	return ++myRefCount;
}

inline int CFireEmitter::DecRef()
{
	return --myRefCount;
}

inline bool CFireEmitter::IsInitialized() const
{
	return myEffect != nullptr;
}
