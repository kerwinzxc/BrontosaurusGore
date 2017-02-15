#pragma once

namespace CU
{
	template<typename T>
	class Vector2;
	using Vector2f = Vector2<float>;
}

struct SFireEmitterData
{
	std::string myNoiseTexturePath;
	std::string myColorTexturePath;
	std::string myAlphaTexturePath;
	CU::Vector2f myDistortions[3];
	float myScrollSpeeds[3];
	float myScales[3];
	float myDistortionScale;
	float myDistortionBias;
};
