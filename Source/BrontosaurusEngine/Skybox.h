#pragma once
#include <vector4.h>
#include <matrix44.h>
#include "Texture.h"

class CEffect;
class CTexture;

struct ID3D11Buffer;
struct ID3D11ShaderResourceView;


namespace CU
{
	class Camera;
}


class CSkybox
{
public: 
	

public:
	CSkybox();
	CSkybox(const CSkybox& aCopy) = delete;
	~CSkybox();

	void Init(const char* aPath);
	void Init(ID3D11ShaderResourceView* aSRV);

	void Render(const CU::Camera & aCamera);
	inline CTexture* GetTexture();
	inline short AddRef();
	inline short DecRef();

private:
	void CreateVertexIndexBuffer();
	void UpdateCbuffer(const CU::Camera& aCamera);

	ID3D11ShaderResourceView* mySRV;
private:
	CEffect* myEffect;
	ID3D11Buffer* myVSBuffer;
	ID3D11Buffer* myPSBuffer;
	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myIndexBuffer;

	CTexture* mySkyboxTexture;

	short myRefCount;
};

inline CTexture * CSkybox::GetTexture()
{
	return mySkyboxTexture;
}

inline short CSkybox::AddRef()
{
	return ++myRefCount;
}

inline short CSkybox::DecRef()
{
	return --myRefCount;
}
