#pragma once
#include <dxgiformat.h>

namespace CU
{
	template <typename T>
	class Vector2;
	using Vector2f = Vector2<float>;
	using Vector2ui = Vector2<unsigned int>;
}

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct D3D11_VIEWPORT;

class CFullScreenHelper;


class CRenderPackage
{
public:
	friend CFullScreenHelper;

	CRenderPackage();
	~CRenderPackage();
	void Init(const CU::Vector2ui& aSize, ID3D11Texture2D* aTexture = nullptr, DXGI_FORMAT aFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	void Clear();
	void Activate();
	void Activate(CRenderPackage & aRenderPackage);
	ID3D11ShaderResourceView*& GetDepthResource();
	ID3D11ShaderResourceView*& GetResource();
	ID3D11RenderTargetView*& GetRenderTargetView();
	ID3D11DepthStencilView*& GetDepthStencilView();

	ID3D11Texture2D*& GetTexture();

	void UpdateTexture(ID3D11Texture2D* aTexture);
	
	CU::Vector2f GetSize();




	inline bool IsInit();
	void operator= (const CRenderPackage& aLeft);
private:
	void CreateTexture2D(const int aWidth, const int aHeight, DXGI_FORMAT aFormat);

private:
	ID3D11Texture2D* myTexture;

	ID3D11ShaderResourceView* myDepthResource;
	ID3D11ShaderResourceView* myResource;
	ID3D11RenderTargetView* myTarget;
	ID3D11DepthStencilView* myDepth;
	D3D11_VIEWPORT* myViewport;
};

inline bool CRenderPackage::IsInit()
{
	return myResource != nullptr;
}

