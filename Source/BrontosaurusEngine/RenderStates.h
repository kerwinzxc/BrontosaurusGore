#pragma once

enum class eRasterizerState
{
	eDefault,
	eNoCulling,
	eNoCullingClockwise,
	eWireFrame,
	eCullFront,
	eSize,
};

enum class eBlendState
{
	eAlphaBlend,
	eNoBlend,
	eAddBlend,
	eMulBlend,
	eOverlay,
	eEmilBlend,
	eEndBlend,
	eSize,
};

enum class eDepthStencilState
{
	eDefault,
	eLesser,
	eReadOnly,
	eDisableDepth,
	eSize,
};

enum class eSamplerState
{
	eClamp,
	eWrap,
	eClamp0Wrap1, // i wanna use both, mvh carl
	eDeferred,
	eSize,
};

