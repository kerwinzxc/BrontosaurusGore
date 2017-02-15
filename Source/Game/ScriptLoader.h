#pragma once

namespace SSlua
{
	class LuaWrapper;
}

namespace ScriptLoader
{
	void RegisterLuaFunctions(SSlua::LuaWrapper& aLuaWrapper);
}
