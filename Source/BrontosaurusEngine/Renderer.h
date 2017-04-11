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
#include "ParticleRenderer.h"
#include "C2DGUIRenderer.h"
#include "ColorGrader.h"

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
	friend CParticleRenderer;
	friend CDeferredRenderer;
	friend C2DGUIRenderer;
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
	const CU::Camera& GetCamera();
	void SetStates(const SChangeStatesMessage* aState);
private:
	void HandleRenderMessage(SRenderMessage* aRenderMesage, int& aDrawCallCount);
	void RenderCameraQueue(SRenderCameraQueueMessage* msg, int & aDrawCallCount);

	void Bloom();
	void HDR();
	void MotionBlur();
	void AntiAliasing();
	void Downsample(CRenderPackage& aRenderPackage);
	void RenderGUI();
	void LensDistortion(CRenderPackage& AddRenderMessage);

	void InitPackages();

	void CreateOncePerFrameBuffer();
	void CreateShadowBuffer();
	void UpdateBuffer();
	void UpdateShadowBuffer(SSetShadowBuffer* msg);

	//void UpdateBuffer(SSetShadowBuffer * msg);

	void CreateRasterizerStates();
	void CreateBlendStates();
	void CreateDepthStencilStates();
	void CreateSamplerStates();


	void DoRenderQueue();


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

private:

	CColorGrader myColorGrader;

	CDeferredRenderer myDeferredRenderer;
	CParticleRenderer myParticleRenderer;

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
	ID3D11Buffer* myShadowBuffer;
	CU::TimerHandle myOncePerFrameBufferTimer;
	CU::TimerHandle myFireTimer;
	bool myIsRunning;

	C2DGUIRenderer myGUIRenderer;
};

inline SRendererSettings& CRenderer::GetSettings()
{
	return mySettings;
}

inline bool CRenderer::GetIsRunning()
{
	return myIsRunning;
}
