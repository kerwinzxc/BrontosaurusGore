#pragma once

struct SFireEmitterData
{
	std::string myNoiseTexturePath;
	std::string myColorTexturePath;
	std::string myAlphaTexturePath;
	float myScrollSpeeds[3];
	float myScales[3];
	CU::Vector2f myDistortions[3];
	float myDistortionScale;
	float myDistortionBias;
};
