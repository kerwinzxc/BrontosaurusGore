#pragma once
#include "../CommonUtilities/StaticArray.h"
#include "Synchronizer.h"
#include "RenderStates.h"
#include "RenderMessages.h"
#include "RenderPackage.h"
#include "FullScreenHelper.h"
#include <TimerManager.h>

//temp includes
#include "../PostMaster/Subscriber.h" //mvh carl
#include "InstanceID.h"
#include "binary_tree.h"
#include "binary_tree.h"
#include "binary_tree.h"

struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;
struct ID3D11Buffer;
struct ID3D11SamplerState;


struct SRendererSettings
{
	bool Bloom;
	bool HDR;
	bool Motionblur;
	bool CromaticAberration;
	bool AA;
	char AALevel;
	bool MSAA;
};


class CRenderer : public Subscriber
{
public:
	CRenderer();
	~CRenderer();
	void Shutdown();
	void AddRenderMessage(SRenderMessage* aRenderMessage);

	void DoLight();
	void Render();
	void SwapWrite();
	void ClearRenderQueue();
	inline bool GetIsRunning();
	inline SRendererSettings& GetSettings();

	inline const CU::Camera& GetCamera();
	void SetLights(CU::GrowingArray<CPointLightInstance>&);
private:
	void HandleRenderMessage(SRenderMessage * aRenderMesage, int & aDrawCallCount);
	
	void DoDefered();

	void Bloom();
	void HDR();
	void MotionBlur();
	void Downsample(CRenderPackage& aRenderPackage);
	void RenderGUI();
	void LensDistortion(CRenderPackage& AddRenderMessage);


	void InitPackages();

	void CreateBuffer();
	void UpdateBuffer();


	void UpdatePointlightBuffer(const CPointLightInstance& aPointLightInstance);
	void UpdateBuffer(SSetShadowBuffer * msg);

	void CreateRasterizerStates();
	void CreateBlendStates();
	void CreateDepthStencilStates();
	void CreateSamplerStates();


	void DoRenderQueue();

	void SetStates(const SChangeStatesMessage* aState);
private:
	class GBuffer
	{
	public:
		void Init();
		
		void SetBuffer();
		void Clear();
		void SetAsResource();
		CRenderPackage& GetAlbedo();
		CRenderPackage& GetNormal();
		ID3D11ShaderResourceView* GetDepth();
	protected:
		CRenderPackage albedoPackage;
		CRenderPackage normalPackage;
		CRenderPackage depthPackage;
	}gBuffer;
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
		CU::StaticArray<CRenderPackage, 3> myDownScalePackages;
		bool myShouldDownScale;
	} myGUIData;

private:


	CSynchronizer<SRenderMessage*> mySynchronizer;
	CSynchronizer<CU::GrowingArray<CPointLightInstance, InstanceID>*> myLightSynchronizer;
	CRenderPackage* renderTo;

	CU::StaticArray< ID3D11RasterizerState*, static_cast<int>(eRasterizerState::eSize)> myRasterizerStates;
	CU::StaticArray< ID3D11DepthStencilState*, static_cast<int>(eDepthStencilState::eSize)> myDepthStencilStates;
	CU::StaticArray< ID3D11BlendState*, static_cast<int>(eBlendState::eSize)> myBlendStates;
	CU::StaticArray< ID3D11SamplerState*, static_cast<int>(eSamplerState::eSize)> mySamplerStates;
	
	CU::TimerManager myTimers;

	CRenderPackage myIntermediatePackage;
	CRenderPackage myBackBufferPackage;

	CFullScreenHelper myFullScreenHelper;
	SRendererSettings mySettings;

	CU::Camera myCamera;

	ID3D11Buffer* myOncePerFrameBuffer;

	CU::TimerHandle myOncePerFrameBufferTimer;
	CU::TimerHandle myFireTimer;
	bool myIsRunning;
	ID3D11Buffer* myPointlightBuffer;
	// Inherited via Subscriber
	eMessageReturn Recieve(const Message& aMessage) override;
};

inline SRendererSettings& CRenderer::GetSettings()
{
	return mySettings;
}

inline bool CRenderer::GetIsRunning()
{
	return myIsRunning;
}

inline const CU::Camera & CRenderer::GetCamera()
{
	return myCamera;
}

