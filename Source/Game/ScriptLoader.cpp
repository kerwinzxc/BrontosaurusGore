#include "stdafx.h"
#include "ScriptLoader.h"
#include "../LuaWrapper/SSlua/SSlua.h"

#include "GlobalLuaFunctions.h"
#include "ComponentMessageTypeToLua.h"

namespace SSlua
{
	typedef CU::GrowingArray<SSArgument> ArgumentList;
	typedef std::function<ArgumentList(const ArgumentList&)> LuaCallbackFunction;
}

void ScriptLoader::RegisterLuaFunctions(SSlua::LuaWrapper& aLuaWrapper)
{
	aLuaWrapper.RegisterFunction(ComponentGetParent, "GetParent", "Argument: component id (number), Return value: the component's parent's id. Returns 0 (zero) if it fails", false);
	aLuaWrapper.RegisterFunction(GameObjectGetPosition, "GetPosition", "Argument: game object id, Return value: numbers x, y, z ", true);
	aLuaWrapper.RegisterFunction(ComponentSubscribeToMessage, "SubscribeToMessage", "First argument: component id (number), second argument: messagetype (number), third argument: name of callback function to call in lua (string). Returns nothing. The callback function must take two arguments, where the first is the component id, and the second is a pointer to the message data", false);
	aLuaWrapper.RegisterFunction(ComponentNotifyParent, "NotifyParent", "First arument: component id (number), second argument: message type (number), third argument (optional): the data you want to send (number, string, vector2/3 whatever). Returns nothing", false);
	aLuaWrapper.RegisterFunction(GetMessageData, "GetMessageData", "First argument: pointer to the message data ('lightuserdata'), second argument: type of data (string). Returns the data specified as second argument", false);
	aLuaWrapper.RegisterFunction(GetMessageType, "GetMessageType", "Argument: string representing a message type, return value: number that is the actual message type. Returns -1 if the string doesn't match any id. Use this when subscribing to messages", true);
	aLuaWrapper.RegisterFunction(SpawnParticles, "SpawnParticles", "First argument: effect type (string), second argument: position to spawn at (three numbers xyz). Example: SpawnParticles(\"blood\", GetPosition(GetParent()))", true);
	aLuaWrapper.RegisterFunction(ComponentGetParentPosition, "GetParentPosition", "Get the position of the component's gameobject. First argument: component id(number)", false);
	aLuaWrapper.RegisterFunction(ChangeLevel, "ChangeLevel", "Changes to level specified with either level name or level index in LevelList.json", true);
	aLuaWrapper.RegisterFunction(QuitGame, "QuitGame", "Quits game, Takes no arguments and returns nothing", true);
	aLuaWrapper.RegisterFunction(IsPlayer, "IsPlayer", "Takes a component id as argument, returns true if it is the player, false if it is not", true);
	aLuaWrapper.RegisterFunction(LuaAssert, "Assert", "Arguments: bool, string (order doesn't matter). If bool is false, crashes the game and shows error message.\nDoes not crash in retail. If only string is provided, it crashes", false);
	aLuaWrapper.RegisterFunction(SetUserData, "SetUserData", "First argument (string): the key you want to set, second argument (any type): the value to set it to", false);

	aLuaWrapper.DoString("data = {}; data[0] = {}");

#ifndef _RETAIL_BUILD

	aLuaWrapper.PrintDocumentation();
	ComponentMessage::PrintComponentsToFile("Script/Documentation/ComponentMessageTypes.txt");

#endif // !_RETAIL_BUILD
}
