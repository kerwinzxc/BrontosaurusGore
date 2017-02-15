#pragma once

enum class eRasterizerState
{
	eDefault,
	eNoCulling,
	eNoCullingClockwise,
	eWireFrame,
	eSize,
};

enum class eBlendState
{
	eAlphaBlend,
	eNoBlend,
	eSize,
};

enum class eDepthStencilState
{
	eDefault,
	eReadOnly,
	eDisableDepth,
	eSize,
};

enum class eSamplerState
{
	eClamp,
	eWrap,
	eClamp0Wrap1, // i wanna use both, mvh carl
	eSize,
};

