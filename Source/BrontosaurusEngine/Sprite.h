#pragma once

class CSpriteInstance;
class CEffect;
class CSurface;
class CDXFramework;
class CLoaderModel;
struct ID3D11Buffer;

namespace CU
{
	template <typename T>
	class Vector2;
	using Vector2f = Vector2<float>;

	template <typename T>
	class Vector4;
	using Vector4f = Vector4<float>;
}

class CSprite
{
public:
	CSprite();
	~CSprite();

	CSprite& operator=(const CSprite& aSprite);
	CSprite& operator=(CSprite&& aSprite);
	
	void Init(const char* aTexturePath);
	void Render(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const CU::Vector2f& aPivot, const CU::Vector4f& aRect, const CU::Vector4f& aColor);
	CU::Vector2f GetTextureSizeFloat() const;
private:
	void CreateEffect();
	void CreateSurface(const char* aTexturePath);
	bool InitBuffers();

	void UpdateAndSetVertexConstantBuffer(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const CU::Vector2f& aPivot, const CU::Vector4f& aRectconst, const CU::Vector4f& aColor);

	CEffect* myEffect;
	CSurface* mySurface;

	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myVertexConstantBuffer;
};
