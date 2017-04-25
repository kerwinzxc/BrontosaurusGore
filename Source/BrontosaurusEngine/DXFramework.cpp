#include "stdafx.h"
#include "DXFramework.h"
#include "picojson.h"

CDXFramework::CDXFramework()
{
	mySwapchain					= nullptr;
	myDevice					= nullptr;
	myDeviceContext				= nullptr;
	myRenderTargetView			= nullptr;
	myDepthStencilView			= nullptr;
	myUseVsync					= false;
}

CDXFramework::~CDXFramework()
{
	Shutdown();
}

#define INFO_PRINT DL_PRINT

bool CDXFramework::CollectAdapters(CU::Vector2<unsigned int> aWindowSize, CU::Vector2<int>& aNumDenumerator, IDXGIAdapter*& outAdapter)
{
	HRESULT result = S_OK;
	IDXGIFactory* factory;

	DXGI_MODE_DESC* displayModeList = nullptr;
	unsigned int numModes = 0;
	//unsigned int i = 0;
	unsigned int denominator = 0;
	unsigned int numerator = 0;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}
	// Use the factory to create an adapter for the primary graphics interface (video card).
	IDXGIAdapter* usingAdapter = nullptr;
	int adapterIndex = 0;
	std::vector<DXGI_ADAPTER_DESC> myAdapterDescs;
	std::vector<IDXGIAdapter*> myAdapters;
	while (factory->EnumAdapters(adapterIndex, &usingAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		usingAdapter->GetDesc(&adapterDesc);
		myAdapterDescs.push_back(adapterDesc);
		myAdapters.push_back(usingAdapter);
		++adapterIndex;
	}

	if (adapterIndex == 0)
	{
		return false;
	}

	INFO_PRINT("%s", "Video card(s) detected: ");
	for (DXGI_ADAPTER_DESC desc : myAdapterDescs)
	{
		int memory = (int)(desc.DedicatedVideoMemory / 1024 / 1024);
		INFO_PRINT("	%ls%s%i%s", desc.Description, " Mem: ", memory, "Mb");
	}

	DXGI_ADAPTER_DESC usingAdapterDesc = myAdapterDescs[0];
	usingAdapter = myAdapters[0];

	INFO_PRINT("%s", "Detecting best card...");


	const std::wstring nvidia = L"NVIDIA";
	const std::wstring ati = L"ATI";

	int memory = (int)(usingAdapterDesc.DedicatedVideoMemory / 1024 / 1024);
	int mostMem = 0;

	for (unsigned int i = 0; i < myAdapterDescs.size(); i++)
	{
		DXGI_ADAPTER_DESC desc = myAdapterDescs[i];
		memory = (int)(desc.DedicatedVideoMemory / 1024 / 1024);
		std::wstring name = desc.Description;
		if (name.find(nvidia) != std::wstring::npos || name.find(ati) != std::wstring::npos)
		{
			if (memory > mostMem)
			{
				mostMem = memory;
				usingAdapterDesc = desc;
				usingAdapter = myAdapters[i];
			}
		}
	}

	INFO_PRINT("%s%ls%s%i", "Using graphic card: ", usingAdapterDesc.Description, " Dedicated Mem: ", mostMem);

	// Enumerate the primary adapter output (monitor).
	IDXGIOutput* pOutput = nullptr;
	if (usingAdapter->EnumOutputs(0, &pOutput) != DXGI_ERROR_NOT_FOUND)
	{
		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		result = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if (!FAILED(result))
		{
			// Create a list to hold all the possible display modes for this monitor/video card combination.
			displayModeList = new DXGI_MODE_DESC[numModes];
			if (displayModeList)
			{
				// Now fill the display mode list structures.
				result = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
				if (!FAILED(result))
				{
					// Now go through all the display modes and find the one that matches the screen width and height.
					// When a match is found store the numerator and denominator of the refresh rate for that monitor.
					for (unsigned int i = 0; i < numModes; i++)
					{
						if (displayModeList[i].Width == (unsigned int)aWindowSize.x)
						{
							if (displayModeList[i].Height == (unsigned int)aWindowSize.y)
							{
								numerator = displayModeList[i].RefreshRate.Numerator;
								denominator = displayModeList[i].RefreshRate.Denominator;
							}
						}
					}
				}
			}
		}
		// Release the adapter output.
		pOutput->Release();
		pOutput = 0;
	}


	// Get the adapter (video card) description.
	result = usingAdapter->GetDesc(&usingAdapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	int myVideoCardMemory = (int)(usingAdapterDesc.DedicatedVideoMemory / 1024 / 1024);
	DL_PRINT("Video card memory: %d", myVideoCardMemory);
	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;



	// Release the factory.
	factory->Release();
	factory = 0;

	if (myUseVsync)
	{
		aNumDenumerator.x = numerator;
		aNumDenumerator.y = denominator;
	}
	else
	{
		aNumDenumerator.x = 0;
		aNumDenumerator.y = 1;
	}

	outAdapter = usingAdapter;
	return true;
}

bool CDXFramework::Initialize(const int aWidth, const int aHeight, const bool aIsFullScreen, const bool aUseVsync, HWND aHWND)
{
	myUseVsync = aUseVsync;
	HRESULT result;
	
	//Create device and swapchain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = aWidth;
	swapChainDesc.BufferDesc.Height = aHeight;

	IDXGIAdapter* adapter = nullptr;
	CU::Vector2<int> numDenum;
	CU::Vector2<unsigned int> windowSize(aWidth, aHeight);
	if (CollectAdapters(windowSize, numDenum, adapter))
	{
		INFO_PRINT("%s%s", "VSYNC Compatible: Yes, Enabled: ", (myUseVsync ? "Yes" : "No"));
	}

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = numDenum.x;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = numDenum.y;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aHWND;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0; // 
	swapChainDesc.Windowed = !aIsFullScreen;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create Devive and Swap chain.

	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined (DEBUG) || defined (_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &mySwapchain, &myDevice, NULL, &myDeviceContext);
	CHECK_RESULT(result, "Failed to create device & swapchain.");

	// Create Render-target-view
	ID3D11Texture2D* backBuffer = nullptr;	
	result = mySwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer); 
	CHECK_RESULT(result, "Failed to get Backbuffer.");

	ID3D11ShaderResourceView* test = nullptr;
	CreateDepthStencil(aWidth, aHeight, myDepthStencilView, test);
	test->Release();


	result = myDevice->CreateRenderTargetView(backBuffer, NULL, &myRenderTargetView); //instead of null we can pass a D3D11_RENDERTARGETVIEW_DESC.
	backBuffer->Release();
	CHECK_RESULT(result, "Failed to create Render Target View.");
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	myDeviceContext->OMSetRenderTargets(1, &myRenderTargetView, myDepthStencilView);

	// Create Viewport
	D3D11_VIEWPORT viewPort;
	viewPort.Width = (FLOAT)aWidth;
	viewPort.Height = (FLOAT)aHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	myDeviceContext->RSSetViewports(1, &viewPort);
	
	return true;
}

void CDXFramework::SetFullscreen()
{
	mySwapchain->SetFullscreenState(true, nullptr);
}

void CDXFramework::SetWindowed()
{
	mySwapchain->SetFullscreenState(false, nullptr);
}

void CDXFramework::DisableDepthStencil()
{
	myDeviceContext->OMSetRenderTargets(1, &myRenderTargetView, NULL);
}

void CDXFramework::EnableDepthStencil()
{
	myDeviceContext->OMSetRenderTargets(1, &myRenderTargetView, myDepthStencilView);
}

void CDXFramework::ClearScreen()
{
	float clearColour[4] = { 0.f, 0.f, 0.0f, 0.f };
	myDeviceContext->ClearRenderTargetView(myRenderTargetView, clearColour);
}

void CDXFramework::ClearDepthStencil()
{
	// Clear the depth buffer.
	myDeviceContext->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}

void CDXFramework::Resize(const unsigned int aWidth, const unsigned int aHeight) //fix this >:3
{
	myRenderTargetView->Release();
	myRenderTargetView = nullptr;

	HRESULT result;

	result = mySwapchain->ResizeBuffers(0, aWidth, aHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	CHECK_RESULT(result, "Couldn't Resize buffer");

	ID3D11Texture2D* backBufferPtr;

	result = mySwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	CHECK_RESULT(result, "Couldn't get backbuffer");

	result = myDevice->CreateRenderTargetView(backBufferPtr, NULL, &myRenderTargetView);
	CHECK_RESULT(result, "Couldn't create render target view");

	ID3D11ShaderResourceView* test = nullptr;
	CreateDepthStencil(aWidth, aHeight, myDepthStencilView, test);
	test->Release();


	backBufferPtr->Release();
	backBufferPtr = nullptr;
	myDeviceContext->OMSetRenderTargets(1, &myRenderTargetView, myDepthStencilView);

	SetViewPort(aWidth, aHeight, 0, 1, 0, 0);
}

void CDXFramework::SetViewPort(const unsigned int aWidth, const unsigned int aHeight, const float aMinDepth, const float aMaxDepth, const float aTopLeftX, const float aTopLeftY)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(aWidth);
	viewport.Height = static_cast<float>(aHeight);
	viewport.MinDepth = aMinDepth;
	viewport.MaxDepth = aMaxDepth;
	viewport.TopLeftX = aTopLeftX;
	viewport.TopLeftY = aTopLeftY;
	myDeviceContext->RSSetViewports(1, &viewport);
}

void CDXFramework::Render()
{
	if (myUseVsync)
	{
		mySwapchain->Present(1, 0);
	}
	else
	{
		mySwapchain->Present(0, 0);
	}
}

void CDXFramework::Shutdown()
{
	mySwapchain->SetFullscreenState(false, nullptr);

	if (mySwapchain != nullptr)
	{
		mySwapchain->Release();
		mySwapchain = nullptr;
	}
	if (myDevice != nullptr)
	{
		myDevice->Release();
		myDevice = nullptr;
	}
	if (myDeviceContext != nullptr)
	{
		myDeviceContext->Release();
		myDeviceContext = nullptr;
	}
	if (myRenderTargetView != nullptr)
	{
		myRenderTargetView->Release();
		myRenderTargetView = nullptr;
	}

	if (myDepthStencilView != nullptr)
	{
		myDepthStencilView->Release();
		myDepthStencilView = nullptr;
	}
}

void CDXFramework::CreateDepthStencil(const int aWidth, const int aHeight, ID3D11DepthStencilView*& aDepthStencilView, ID3D11ShaderResourceView*& aDepthStencilResource, ID3D11Texture2D * aTexture, DXGI_FORMAT aFormat)
{	
	if (aDepthStencilView != nullptr)
		aDepthStencilView->Release();

	ID3D11Texture2D* depthStencilBuffer = nullptr;


	HRESULT result = S_OK;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	// Set up the description of the depth buffer.
	depthBufferDesc.Width = aWidth;
	depthBufferDesc.Height = aHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = GetDepthResourceFormat(aFormat);
	depthBufferDesc.SampleDesc.Count = 1;				//MSAA magic set too 4 for better stuffs
	depthBufferDesc.SampleDesc.Quality = 0;				//MSAA magic set to 1 to enable magic
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	depthStencilBuffer = aTexture;
	if(depthStencilBuffer == nullptr)
	{
		result = myDevice->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	}
	CHECK_RESULT(result, "Failed to create Texture2D.");
	
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = aFormat;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // Set to D3D11_DSV_DIMENSION_TEXTURE2DMS for some sort of multiSampling whaop
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	// Create the depth stencil view.
	result = myDevice->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &aDepthStencilView);
	CHECK_RESULT(result, "Failed to create Depth Stencil View.");
	//myDeviceContext->OMSetRenderTargets(1, &myRenderTargetView, aDepthStencilView);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = GetDepthSRVFormat(aFormat);
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;

	result = myDevice->CreateShaderResourceView(depthStencilBuffer, &srvd, &aDepthStencilResource);
	CHECK_RESULT(result, "Failed to create Shader Resouce View.");

	depthStencilBuffer->Release();
}


DXGI_FORMAT CDXFramework::GetDepthResourceFormat(DXGI_FORMAT depthformat)
{
	DXGI_FORMAT resformat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	switch (depthformat)
	{
	case DXGI_FORMAT::DXGI_FORMAT_D16_UNORM:
		resformat = DXGI_FORMAT::DXGI_FORMAT_R16_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT:
		resformat = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT:
		resformat = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		resformat = DXGI_FORMAT::DXGI_FORMAT_R32G8X24_TYPELESS;
		break;
	}

	return resformat;
}

DXGI_FORMAT CDXFramework::GetDepthSRVFormat(DXGI_FORMAT depthformat)
{
	DXGI_FORMAT srvformat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	switch (depthformat)
	{
	case DXGI_FORMAT::DXGI_FORMAT_D16_UNORM:
		srvformat = DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT:
		srvformat = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT:
		srvformat = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		srvformat = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		break;
	}
	return srvformat;
}
