#include "stdafx.h"
#include "LoadFireComponent.h"

#include "..\Components\FireComponent.h"
#include "..\BrontosaurusEngine\FireEmitterInstance.h"
#include "..\BrontosaurusEngine\FireEmitterData.h"

int LoadFireFromJson(const std::string& aJsonPath, CScene& aScene);

int LoadFireComponent(KLoader::SLoadedComponentData someData)
{
	LoadManager* loadManager = LoadManager::GetInstance();
	if (!loadManager) return NULL_COMPONENT;
	CScene& scene = loadManager->GetCurrentScene();

	if (someData.myData.HasKey("jsonPath"))
	{
		const std::string& jsonPath = someData.myData["jsonPath"].GetString();
		return LoadFireFromJson(jsonPath, scene);
	}

	CFireEmitterInstance fireEmitter;
	SFireEmitterData fireData;

	fireData.myNoiseTexturePath = someData.myData["NoiseTexturePath"].GetString();
	fireData.myColorTexturePath = someData.myData["ColorTexturePath"].GetString();
	fireData.myAlphaTexturePath = someData.myData["AlphaTexturePath"].GetString();

	fireData.myScrollSpeeds[0] = /*1.3f;*/someData.myData["ScrollSpeeds"][0].GetFloat();
	fireData.myScrollSpeeds[1] = /*2.1f;*/ someData.myData["ScrollSpeeds"][1].GetFloat();
	fireData.myScrollSpeeds[2] = /*2.3f;*/ someData.myData["ScrollSpeeds"][2].GetFloat();

	fireData.myScales[0] = /*1.f;*/ someData.myData["Scales"][0].GetFloat();
	fireData.myScales[1] = /*2.f;*/ someData.myData["Scales"][1].GetFloat();
	fireData.myScales[2] = /*3.f;*/ someData.myData["Scales"][2].GetFloat();

	CU::CJsonValue distortions = someData.myData["Distortions"];

	fireData.myDistortions[0].Set(/*0.1f, 0.2f);*/ distortions[0]["x"].GetFloat(), distortions[0]["y"].GetFloat());
	fireData.myDistortions[1].Set(/*0.1f, 0.3f);*/ distortions[1]["x"].GetFloat(), distortions[1]["y"].GetFloat());
	fireData.myDistortions[2].Set(/*0.1f, 0.1f);*/ distortions[2]["x"].GetFloat(), distortions[2]["y"].GetFloat());

	fireData.myDistortionScale = /*0.8f*/ someData.myData["DistortionScale"].GetFloat();
	fireData.myDistortionBias = /*0.5f*/ someData.myData["DistortionBias"].GetFloat();

	fireEmitter.Init(fireData);
	fireEmitter.GetTransformation().Rotate(/*3.141592f*/someData.myData["RotationY"].GetFloat(), CU::Axees::Y);
	float scale = someData.myData["Scale"].GetFloat();
	fireEmitter.GetTransformation().m11 *= /*2.f;*/ scale;
	fireEmitter.GetTransformation().m22 *= /*2.f;*/ scale;
	fireEmitter.GetTransformation().m33 *= /*2.f;*/ scale;

	InstanceID id = scene.AddFireEmitters(fireEmitter);

	CFireComponent* component = new CFireComponent(scene, id);

	return component->GetId();
}

int LoadFireFromJson(const std::string& aJsonPath, CScene& aScene)
{
	CU::CJsonValue fireDocument(aJsonPath);
	CFireEmitterInstance fireEmitter;
	SFireEmitterData fireData;

	fireData.myNoiseTexturePath = fireDocument["NoiseTexturePath"].GetString();
	fireData.myColorTexturePath = fireDocument["ColorTexturePath"].GetString();
	fireData.myAlphaTexturePath = fireDocument["AlphaTexturePath"].GetString();

	fireData.myScrollSpeeds[0] = /*1.3f;*/fireDocument["ScrollSpeeds"][0].GetFloat();
	fireData.myScrollSpeeds[1] = /*2.1f;*/ fireDocument["ScrollSpeeds"][1].GetFloat();
	fireData.myScrollSpeeds[2] = /*2.3f;*/ fireDocument["ScrollSpeeds"][2].GetFloat();

	fireData.myScales[0] = /*1.f;*/ fireDocument["Scales"][0].GetFloat();
	fireData.myScales[1] = /*2.f;*/ fireDocument["Scales"][1].GetFloat();
	fireData.myScales[2] = /*3.f;*/ fireDocument["Scales"][2].GetFloat();

	CU::CJsonValue distortions = fireDocument["Distortions"];

	fireData.myDistortions[0].Set(/*0.1f, 0.2f);*/ distortions[0]["x"].GetFloat(), distortions[0]["y"].GetFloat());
	fireData.myDistortions[1].Set(/*0.1f, 0.3f);*/ distortions[1]["x"].GetFloat(), distortions[1]["y"].GetFloat());
	fireData.myDistortions[2].Set(/*0.1f, 0.1f);*/ distortions[2]["x"].GetFloat(), distortions[2]["y"].GetFloat());

	fireData.myDistortionScale = /*0.8f*/ fireDocument["DistortionScale"].GetFloat();
	fireData.myDistortionBias = /*0.5f*/ fireDocument["DistortionBias"].GetFloat();

	fireEmitter.Init(fireData);
	fireEmitter.GetTransformation().Rotate(/*3.141592f*/fireDocument["RotationY"].GetFloat(), CU::Axees::Y);
	float scale = fireDocument["Scale"].GetFloat();
	fireEmitter.GetTransformation().m11 *= /*2.f;*/ scale;
	fireEmitter.GetTransformation().m22 *= /*2.f;*/ scale;
	fireEmitter.GetTransformation().m33 *= /*2.f;*/ scale;

	InstanceID id = aScene.AddFireEmitters(fireEmitter);

	CFireComponent* component = new CFireComponent(aScene, id);

	return component->GetId();
}
