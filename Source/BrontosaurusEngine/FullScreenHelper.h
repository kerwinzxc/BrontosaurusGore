#pragma once

#include "../CommonUtilities/vector2.h"
#include "../CommonUtilities/vector4.h"
#include "../CommonUtilities/StaticArray.h"

struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class CEffect;
class CRenderPackage;

class CFullScreenHelper
{
public:
	struct SVertexData
	{
		CU::Vector4f position;
		CU::Vector2f textureCoords;
	};

	enum class eEffectType
	{
		eCopy,
		eCopyR,
		eCopyG,
		eCopyB,
		eHDR,
		eAdd,
		eLuminance,
		eBloominance,
		eGaussianBlurHorizontal,
		eGaussianBlurVertical,
		eLightShafts,
		eAverage,
		eCubicLensDistortion,
		eToneMap,
		eMotionBlur, //ooh
		eDeferredAmbient, //oh oh oh oh
		eDeferredDirectional,
		eDeferredSpotLight,
		eColorGrading,
		eAA,
		eMetaSurface,
		eSSAO,
		eSize,
	};

public:
	CFullScreenHelper();
	~CFullScreenHelper();

	void DoEffect(const eEffectType aEffectType, CRenderPackage* aRenderPackage, CRenderPackage* aSecondRenderPackage = nullptr);
	void DoEffect(const eEffectType aEffectType, const CU::Vector4f & aRect, CRenderPackage* aRenderPackage, CRenderPackage* aSecondRenderPackage = nullptr);

	void DoEffect(const eEffectType aEffectType, ID3D11ShaderResourceView* aRenderPackage, ID3D11ShaderResourceView* aSecondRenderPackage = nullptr);
	void DoEffect(const eEffectType aEffectType, const CU::Vector4f & aRect, ID3D11ShaderResourceView* aRenderPackage, ID3D11ShaderResourceView* aSecondRenderPackage = nullptr);

	void DoEffect(const eEffectType aEffectType);
	void DoEffect(const eEffectType aEffectType, const CU::Vector4f & aRect);

private: 
	void CreateQuad();

private:
	CU::StaticArray<CEffect*, static_cast<int>(eEffectType::eSize)> myEffects;
	ID3D11Buffer* myVertexBuffer;

};

