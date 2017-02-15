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
