#include "stdafx.h"
#include "LuaFunctions.h"
#include "../Components/GameObjectManager.h"
#include "BrontosaurusEngine/Engine.h"
#include "Game/Game.h"
#include "CommonUtilities/DynamicString.h"
#include "..\Components/GameObject.h"
#include "..\Components/Component.h"
#include "..\Components/ComponentManager.h"
#include "Components/ModelComponentManager.h"
#include "Physics/PhysicsManager.h"
#include "BrontosaurusEngine\ModelManager.h"
#include "LoadManager.h"
#include "Scene.h"
#include "Physics/RigidBodyComponent.h"
#include "Game/PlayState.h"
#include "Components\AudioSourceComponentManager.h"

#include "Components/ParticleEmitterComponentManager.h"

void LuaFunctions::RegisterFunctions()
{
	SSlua::LuaWrapper& luaWrapper = SSlua::LuaWrapper::GetInstance();

	luaWrapper.RegisterEngineFunction(SSlua::LuaCallbackFunction(&CreateObject), "CreateObject");
	luaWrapper.RegisterEngineFunction(SSlua::LuaCallbackFunction(&AddComponent), "AddComponent");
	luaWrapper.RegisterEngineFunction(SSlua::LuaCallbackFunction(&CreateModel), "CreateModel");
	luaWrapper.RegisterEngineFunction(SSlua::LuaCallbackFunction(&SetCubeMap), "SetSkyBox");

	luaWrapper.RegisterEngineFunction(SSlua::LuaCallbackFunction(&CreateCameraComponent), "CreateCamera");
	luaWrapper.RegisterEngineFunction(SSlua::LuaCallbackFunction(&CreateAudioSourceComponent), "CreateAudioSource");
	luaWrapper.RegisterEngineFunction(SSlua::LuaCallbackFunction(&CreateControllerComponent), "LoadControllerComponent");
	luaWrapper.RegisterEngineFunction(SSlua::LuaCallbackFunction(&CreateParticleEmitterComponent), "CreateParticleEmitter");
}

SSlua::ArgumentList LuaFunctions::CreateObject(SSlua::ArgumentList anArgumentList)
{
	if (anArgumentList.Size() != 2)
	{
		DL_ASSERT((CU::DynamicString("Lua engine error: Engine.CreateObject expected 2 arguments (string,table) got ") + anArgumentList.Size()).c_str());
	}

	if (anArgumentList[0].GetType() != eSSType::STRING)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected string as first argument got ");
		errorString += anArgumentList[0].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}

	const CU::DynamicString name(anArgumentList[0].GetString());

	if (anArgumentList[1].GetType() != eSSType::TABLE)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}

	const ssLuaTable& luaMatrix = anArgumentList[1].GetTable();
	CU::Matrix44<ssLuaNumber> transform;

	transform.m11 = luaMatrix.at("m11").GetNumber();
	transform.m12 = luaMatrix.at("m12").GetNumber();
	transform.m13 = luaMatrix.at("m13").GetNumber();
	transform.m14 = luaMatrix.at("m14").GetNumber();

	transform.m21 = luaMatrix.at("m21").GetNumber();
	transform.m22 = luaMatrix.at("m22").GetNumber();
	transform.m23 = luaMatrix.at("m23").GetNumber();
	transform.m24 = luaMatrix.at("m24").GetNumber();

	transform.m31 = luaMatrix.at("m31").GetNumber();
	transform.m32 = luaMatrix.at("m32").GetNumber();
	transform.m33 = luaMatrix.at("m33").GetNumber();
	transform.m34 = luaMatrix.at("m34").GetNumber();

	transform.m41 = luaMatrix.at("m41").GetNumber();
	transform.m42 = luaMatrix.at("m42").GetNumber();
	transform.m43 = luaMatrix.at("m43").GetNumber();
	transform.m44 = luaMatrix.at("m44").GetNumber();

	CGameObject* anObjectPointer = nullptr;// myGameObjectManager->CreateGameObject(transform);

	SSArgument returnArgument;
	returnArgument = static_cast<ssLuaNumber>(anObjectPointer->GetId());

	SSlua::ArgumentList returnArguments;
	returnArguments.Init(1);

	returnArguments.Add(returnArgument);
	return returnArguments;
}

SSlua::ArgumentList LuaFunctions::AddComponent(SSlua::ArgumentList anArgumentList)
{
	SSlua::LuaWrapper::CheckArguments("AddComponent", { eSSType::NUMBER, eSSType::NUMBER }, anArgumentList);

	CComponent* tempComponent = (CComponentManager::GetInstance().GetComponent(static_cast<ComponentId>(anArgumentList[0].GetNumber())));
	CGameObject* gameObject = dynamic_cast<CGameObject*>(tempComponent);
	if (gameObject == nullptr)
	{
		DL_ASSERT("Component at id %f is not an object", anArgumentList[0].GetFloat());
		return{ SSArgument(false) };
	}

	CComponent* component = CComponentManager::GetInstance().GetComponent(static_cast<ComponentId>(anArgumentList[1].GetNumber()));

	gameObject->AddComponent(component);

	return{ SSArgument(true) };
}

SSlua::ArgumentList LuaFunctions::CreateModel(SSlua::ArgumentList anArgumentList)
{
	SSlua::LuaWrapper::CheckArguments("CreateModel", { eSSType::STRING }, anArgumentList);

	CU::DynamicString modelPath(anArgumentList[0].GetString());
	modelPath += ".fbx";

	CModelComponent* component = CModelComponentManager::GetInstance().CreateComponent(modelPath.c_str());
	component->SetVisibility(true);


	return{ SSArgument(static_cast<ssLuaNumber>(component->GetId())) };
}

SSlua::ArgumentList LuaFunctions::SetCubeMap(SSlua::ArgumentList anArgumentList)
{
	SSlua::LuaWrapper::CheckArguments("SetCubeMap", { eSSType::STRING }, anArgumentList);
	LoadManager* loadManager = LoadManager::GetInstance();

	SSlua::ArgumentList tempList;

	if (!loadManager)
	{
		return tempList;
	}
	CScene& currentScene = LoadManager::GetInstance()->GetCurrentScene();

	currentScene.SetSkybox(anArgumentList[0].GetString());

	return tempList;
}

SSlua::ArgumentList LuaFunctions::CreateParticleEmitterComponent(SSlua::ArgumentList anArgumentList)
{

#pragma region craycray
	if (anArgumentList[0].GetType() != eSSType::STRING)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[1].GetType() != eSSType::NUMBER)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[2].GetType() != eSSType::TABLE)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[3].GetType() != eSSType::TABLE)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[4].GetType() != eSSType::NUMBER)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[5].GetType() != eSSType::NUMBER)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[6].GetType() != eSSType::NUMBER)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[7].GetType() != eSSType::NUMBER)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[8].GetType() != eSSType::NUMBER)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[9].GetType() != eSSType::NUMBER)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[10].GetType() != eSSType::NUMBER)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
	if (anArgumentList[11].GetType() != eSSType::NUMBER)
	{
		CU::DynamicString errorString("Lua engine error: Engine.CreateObject expected a table as second argument got ");
		errorString += anArgumentList[1].GetTypeName();
		DL_ASSERT(errorString.c_str());
	}
#pragma endregion

	SEmitterData data;
	data.TexturePath = anArgumentList[0].GetString();
	data.EmissionRate = anArgumentList[1].GetFloat();
	
	ssLuaTable minVel = anArgumentList[2].GetTable();
	data.MinEmissionVelocity.x = minVel["x"].GetFloat();
	data.MinEmissionVelocity.y = minVel["y"].GetFloat();
	data.MinEmissionVelocity.z = minVel["z"].GetFloat();


	ssLuaTable maxVel = anArgumentList[3].GetTable();
	data.MaxEmissionVelocity.x = maxVel["x"].GetFloat();
	data.MaxEmissionVelocity.y = maxVel["y"].GetFloat();
	data.MaxEmissionVelocity.z = maxVel["z"].GetFloat();

	data.MinParticleLifeTime = anArgumentList[4].GetFloat();
	data.MaxParticleLifeTime = anArgumentList[5].GetFloat();

	data.StartSize	 = anArgumentList[6].GetFloat();
	data.EndSize	 = anArgumentList[7].GetFloat();

	data.Gravity = { 0.0f, 0.0f, 0.0f };
	data.UseGravity = false;
	data.StartColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	data.EndColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	data.RotationCurve = eLerpCurve::eSmootherStep;
	data.ColorCurve = eLerpCurve::eSmootherStep;
	data.SizeCurve = eLerpCurve::eSmootherStep;

	data.StartRotation = 0.0f;
	data.EndRotation = 0.0f;

	//data.EmissonLifeTime = anArgumentList[10].GetFloat();
	data.NumOfParticles = anArgumentList[11].GetInt();

	data.ColorOverLife;

	CParticleEmitterComponent* emitter = CParticleEmitterComponentManager::GetInstance().CreateComponent(data);

	return{ SSArgument(static_cast<ssLuaNumber>(emitter->GetId())) };
}

SSlua::ArgumentList LuaFunctions::CreateControllerComponent(SSlua::ArgumentList anArgumentList)
{
	return anArgumentList;
}

SSlua::ArgumentList LuaFunctions::CreateCameraComponent(SSlua::ArgumentList anArgumentList)
{
	return anArgumentList;
}

SSlua::ArgumentList LuaFunctions::CreateAudioSourceComponent(SSlua::ArgumentList anArgumentList)
{
	CAudioSourceComponent* audio = CAudioSourceComponentManager::GetInstance().CreateComponent();
	return{ SSArgument(static_cast<ssLuaNumber>(audio->GetId())) };

}
