#pragma once
#include <assert.h>
#include <directxmath.h>
#include <d3d11.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define CHECK_RESULT(r, msg, ...) if(FAILED(r)) { DL_ASSERT(msg, __VA_ARGS__); }
#define CHECK_BOOL_RESULT(r, msg, ...) if (r != true) { DL_ASSERT(msg, __VA_ARGS__); }

class CDXFramework
{
public:
	CDXFramework();
	~CDXFramework();
	void ClearScreen();
	void ClearDepthStencil();
	void CreateDepthStencil(const int aWidth, const int aHeight, ID3D11DepthStencilView *& aDepthStencilView, ID3D11ShaderResourceView *& aDepthStencilResource);
	void Resize(const unsigned int aWidth, const unsigned int aHeight);
	void SetViewPort(const unsigned int aWidth, const unsigned int aHeight, const float aMinDepth, const float aMaxDepth, const float aTopLeftX, const float aTopLeftY);
	void Render();
	bool Initialize(const int aWidth, const int aHeight, const bool aIsFullScreen, HWND aHWND);


	void DisableDepthStencil();
	void EnableDepthStencil();

	void Shutdown();
	inline ID3D11Device* GetDevice();
	inline ID3D11DeviceContext* GetDeviceContext();
	inline IDXGISwapChain& GetSwapChain();

private:
	bool CollectAdapters(CU::Vector2<unsigned int> aWindowSize, CU::Vector2<int>& aNumDenumerator, IDXGIAdapter*& outAdapter);
	DXGI_FORMAT GetDepthResourceFormat(DXGI_FORMAT depthformat);
	DXGI_FORMAT GetDepthSRVFormat(DXGI_FORMAT depthformat);

private:
	IDXGISwapChain *mySwapchain;
	ID3D11Device* myDevice;
	ID3D11DeviceContext* myDeviceContext;
	ID3D11RenderTargetView* myRenderTargetView;

	ID3D11DepthStencilView* myDepthStencilView;
};

ID3D11Device* CDXFramework::GetDevice()
{
	return myDevice;
}

ID3D11DeviceContext* CDXFramework::GetDeviceContext()
{
	return myDeviceContext;
}

inline IDXGISwapChain & CDXFramework::GetSwapChain()
{
	return *mySwapchain;
}
