#pragma once
#include "../LuaWrapper/SSlua/SSlua.h"

namespace LuaFunctions
{
	void RegisterFunctions();
	
	SSlua::ArgumentList CreateObject(SSlua::ArgumentList anArgumentList);
	SSlua::ArgumentList AddComponent(SSlua::ArgumentList anArgumentList);
	SSlua::ArgumentList CreateModel(SSlua::ArgumentList anArgumentList);
	SSlua::ArgumentList SetCubeMap(SSlua::ArgumentList anArgumentList);

	SSlua::ArgumentList CreateCameraComponent(SSlua::ArgumentList anArgumentList);
	SSlua::ArgumentList CreateAudioSourceComponent(SSlua::ArgumentList anArgumentList);
	SSlua::ArgumentList CreateControllerComponent(SSlua::ArgumentList anArgumentList);
	SSlua::ArgumentList CreateParticleEmitterComponent(SSlua::ArgumentList anArgumentList);
}
