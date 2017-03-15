#pragma once
#include "../CommonUtilities/StaticArray.h"
#include "Synchronizer.h"
#include "RenderStates.h"
#include "RenderMessages.h"
#include "RenderPackage.h"
#include "DeferredRenderer.h"
#include "FullScreenHelper.h"
#include <TimerManager.h>

//temp includes
#include "../PostMaster/Subscriber.h" //mvh carl
#include "../ThreadedPostmaster/Subscriber.h"

struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;
struct ID3D11Buffer;
struct ID3D11SamplerState;
class CTexture;

struct SRendererSettings
{
	bool Bloom;
	bool HDR;
	bool Motionblur;
	bool CromaticAberration;
	bool AA;
	char AALevel;
	bool MSAA;
	bool FXAA;
};


class CRenderer : public Postmaster::ISubscriber
{
	friend CDeferredRenderer;
public:
	CRenderer();
	~CRenderer();

	void Shutdown();
	void AddRenderMessage(SRenderMessage* aRenderMessage);

	void Render();
	void SwapWrite();
	void ClearRenderQueue();
	inline bool GetIsRunning();
	inline SRendererSettings& GetSettings();

	//inline const CU::Camera& GetCamera();

private:
	void HandleRenderMessage(SRenderMessage* aRenderMesage, int& aDrawCallCount);
	
	void Bloom();
	void HDR();
	void MotionBlur();
	void AntiAliasing();
	void Downsample(CRenderPackage& aRenderPackage);
	void RenderGUI();
	void LensDistortion(CRenderPackage& AddRenderMessage);

	void InitPackages();

	void CreateBuffer();
	void UpdateBuffer();

	void UpdateBuffer(SSetShadowBuffer * msg);

	void CreateRasterizerStates();
	void CreateBlendStates();
	void CreateDepthStencilStates();
	void CreateSamplerStates();


	void DoRenderQueue();

	void SetStates(const SChangeStatesMessage* aState);

	void DoColorGrading();
private:

	struct SHDRData
	{
		CRenderPackage myInputPackage;
		CRenderPackage myAverageLumPackage;
		CRenderPackage myAverageColPackage;
		CRenderPackage myTonemapPackage;
	} myHDRData;
	struct SBloomData
	{
		CRenderPackage myInputPackage;
		CRenderPackage myLumPackage;
		CRenderPackage myToneMapPackage;
		CU::StaticArray<CRenderPackage, 4> myBloomPackages;
	} myBloomData;
	struct SDownSamples
	{
		CRenderPackage squarePackage;
		CU::GrowingArray<CRenderPackage> downsamplePackages;
	} myDownsampleData; 
	struct SMotionBlurData
	{
		CRenderPackage inputPackage;
		CRenderPackage velocityPackage;
	} myMotionBlurData;
	struct SDistortionData
	{
		CRenderPackage inputPackage;
		CRenderPackage distortionPackage;
	} myDistortionData;
	struct SGUIData
	{
		CRenderPackage myInputPackage;
	} myGUIData;

private:
	CTexture* myLut;

	CRenderPackage myColorGradingPackage;
	CDeferredRenderer myDeferredRenderer;

	CSynchronizer<SRenderMessage*> mySynchronizer;

	CRenderPackage* renderTo;

	CU::StaticArray<ID3D11RasterizerState*, static_cast<int>(eRasterizerState::eSize)> myRasterizerStates;
	CU::StaticArray<ID3D11DepthStencilState*, static_cast<int>(eDepthStencilState::eSize)> myDepthStencilStates;
	CU::StaticArray<ID3D11BlendState*, static_cast<int>(eBlendState::eSize)> myBlendStates;
	CU::StaticArray<ID3D11SamplerState*, static_cast<int>(eSamplerState::eSize)> mySamplerStates;
	
	CU::TimerManager myTimers;

	CRenderPackage myIntermediatePackage;
	CRenderPackage myBackBufferPackage;
	CRenderPackage myAntialiasingPackage;

	CFullScreenHelper myFullScreenHelper;
	SRendererSettings mySettings;

	CU::Camera myCamera;

	ID3D11Buffer* myOncePerFrameBuffer;

	CU::TimerHandle myOncePerFrameBufferTimer;
	CU::TimerHandle myFireTimer;
	bool myIsRunning;
};

inline SRendererSettings& CRenderer::GetSettings()
{
	return mySettings;
}

inline bool CRenderer::GetIsRunning()
{
	return myIsRunning;
}

//inline const CU::Camera & CRenderer::GetCamera()
//{
//	return myCamera;
//}

