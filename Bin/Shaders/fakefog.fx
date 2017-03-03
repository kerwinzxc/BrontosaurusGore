//#include <shader_structs.fx>

static const float fogStart = 10000.0f; // womba
static const float fogEnd 	= 100000000.f;  //after this: max fog?
static const float fogDensity = 10.0f; // 100% unsure about this one lol, removed old variable without saving value hehe

float GetFogFactor(float3 aViewPosition)
{
	float fogFactor = 0.0f;
	fogFactor = saturate((fogEnd - aViewPosition.z) / (fogEnd - fogStart));
	//fogFactor = 1.0f / (exp(aViewPosition.z * fogDensity));
	//fogFactor = 1.0f / (exp(pow(aViewPosition.z * fogDensity, 2)));

	return fogFactor;
}

static const float cVolFogHeightDensityAtViewer = 0.1f;
static const float cHeightFalloff = 0.1f;
static const float cGlobalDensity = 0.25f;

float ComputeVolumetricFog(in float3 cameraToWorldPos)
{
// NOTE: cVolFogHeightDensityAtViewer = exp( -cHeightFalloff * cViewPos.z );
	float fogInt = length(cameraToWorldPos) * cVolFogHeightDensityAtViewer;
	const float cSlopeThreshold = 0.01;
	if (abs(cameraToWorldPos.z) > cSlopeThreshold)
	{
		float t = cHeightFalloff * cameraToWorldPos.z;
		fogInt *= (1.0 - exp(-t)) / t;
	}
	return exp(-cGlobalDensity * fogInt);
}
