#include "stdafx.h"
#include "SSlua.h"
#include "../SSArgument/SSArgument.h"
#include <codecvt>
#include <Windows.h>
#include <iostream>
#include <fstream>


#define ERROR_MESSAGE(message) MessageBeep(MB_ICONERROR); MessageBox(nullptr, message, L"ERROR", MB_OK | MB_ICONERROR);

std::wstring stringToWstring(const std::string& t_str)
{
	//setup converter
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.from_bytes(t_str);
}

namespace SSlua
{
	LuaWrapper* LuaWrapper::ourInstance = nullptr;

	template<>
	void LuaWrapper::Push<int>(const int& aVariable)
	{
		lua_pushinteger(myState, aVariable);
	}

	template<>
	void LuaWrapper::Push<std::string>(const std::string& aVariable)
	{
		lua_pushstring(myState, aVariable.c_str());
	}

	template<>
	void LuaWrapper::Push<void*>(void* const& aVariable)
	{
		lua_pushlightuserdata(myState, aVariable);
	}

	void LuaWrapper::CheckAndCreate()
	{
		if (CheckCreated() == false)
		{
			//if (CheckCreated() == false)
			//{
				ourInstance = new LuaWrapper();
				assert(ourInstance != nullptr && "Failed to create Lua Wrapper");
			//}
		}
	}

	void LuaWrapper::DestroyIfCreated()
	{
		if (CheckCreated() == true)
		{
			delete ourInstance;
			ourInstance = nullptr;
		}
	}

	bool LuaWrapper::CheckCreated()
	{
		return ourInstance != nullptr;
	}

	LuaWrapper& LuaWrapper::GetInstance()
	{
		CheckAndCreate();
		return *ourInstance;
	}
	
	void LuaWrapper::RegisterFunctions(const std::function<void(LuaWrapper&)>& aRegisterFunctionsFunction)
	{
		if (aRegisterFunctionsFunction != nullptr)
		{
			aRegisterFunctionsFunction(*this);
		}
	}

	void LuaWrapper::RegisterFunction(const LuaCallbackFunction &aFunction,const std::string& aName,const std::string& aHelpText, const bool aShouldBeExposedToConsole)
	{
		myVoidFunctions[aName] = aFunction;
		myExposedFunctions[aName] = aHelpText;
		if (aShouldBeExposedToConsole == true)
		{
			myConsoleFunctions[aName] = aFunction;
		}
		lua_pushstring(myState, aName.c_str());
		lua_pushcclosure(myState, StaticLuaVoidCallback, 1);
		lua_setglobal(myState, aName.c_str());
	}

	void LuaWrapper::RegisterEngineFunction(const LuaCallbackFunction& aFunction, const std::string& aName)
	{
		std::string tempName = myEngineTableName + "." + aName;

		lua_getglobal(myState, myEngineTableName.c_str());
		if (lua_type(myState, -1) != LUA_TTABLE)
		{
			lua_pop(myState, 1);
			lua_newtable(myState);
		}

		myVoidFunctions[tempName] = aFunction;

		lua_pushstring(myState, tempName.c_str());
		lua_pushcclosure(myState, StaticLuaVoidCallback, 1);
		lua_setfield(myState, 1, aName.c_str());
		lua_setglobal(myState, myEngineTableName.c_str());
	}

	void LuaWrapper::AddScriptPath(std::string aPath)
	{
		lua_getglobal(myState, "package");
		lua_getfield(myState, -1, "path");
		std::string currentPath = lua_tostring(myState, -1);
		(currentPath += ";") += aPath + "/?.lua";
		lua_pop(myState, 1);
		lua_pushstring(myState, currentPath.c_str());
		lua_setfield(myState, -2, "path");
		lua_pop(myState, 1);
	}

	void LuaWrapper::LoadCode(const std::string& aFileName)
	{
		CheckError(luaL_loadfile(myState, aFileName.c_str()));

		//Alex
		std::wstring file = myFileWatcher.StringToWString(aFileName);
		myFileWatcher.WatchFileChange(file, std::bind(&LuaWrapper::FileChanged, this, std::placeholders::_1));
		//
	}

	void LuaWrapper::RunLoadedCode()
	{
		CheckError(lua_pcall(myState, 0, LUA_MULTRET, 0));
	}

	bool LuaWrapper::DoFile(const std::string& aFileName)
	{
		int error = luaL_dofile(myState, aFileName.c_str());
		return error == LUA_OK;
	}

	bool LuaWrapper::DoString(const std::string& aPieceOfCode)
	{
		int error = luaL_dostring(myState, aPieceOfCode.c_str());
		return error == LUA_OK;
	}

	bool LuaWrapper::LoadLuaString(const std::string& aLuaScript)
	{
		int error = luaL_loadstring(myState, aLuaScript.c_str());
		//CheckError(error);
		return error != LUA_OK;
	}

	void LuaWrapper::GetGlobal(const std::string& aVariableName, int* const aType)
	{
		int type = lua_getglobal(myState, aVariableName.c_str());
		if (aType)
		{
			*aType = type;
		}
	}

	void LuaWrapper::AssignePairToTableAt(const int aTableIndex)
	{
		lua_settable(myState, aTableIndex);
	}

	void LuaWrapper::GetValueInTableAt(const int aTableIndex)
	{
		lua_gettable(myState, aTableIndex);
	}

	void LuaWrapper::GetValueFromTable(const std::string& aTableName, const std::string& aKeyName)
	{
		int type = -1;
		GetGlobal(aTableName, &type);
		if (type != LUA_TTABLE) return;

		Push<std::string>(aKeyName);
		GetValueInTableAt(-2);
	}

	void LuaWrapper::GetLastError(std::string& aErrorMessageOut) const
	{
		if (lua_isstring(myState, -1))
		{
			aErrorMessageOut = lua_tostring(myState, -1);
			lua_pop(myState, 1);
		}
	}

	bool LuaWrapper::ParseLuaTable(const std::string& aScriptPath, const std::string& aTableName, std::map<std::string, SSArgument>& aTableMapOut)
	{
		LoadCode(aScriptPath);
		RunLoadedCode();

		lua_getglobal(myState, aTableName.c_str());
		if (lua_istable(myState, -1) == false)
		{
			DL_MESSAGE_BOX("Failed to get lua table %s in file %s", aTableName.c_str(), aScriptPath.c_str());
			DL_ASSERT("Failed to get lua table %s in file %s", aTableName.c_str(), aScriptPath.c_str());
		}
		
		lua_pushnil(myState);

		bool result = true;
		while (lua_next(myState, -2))
		{
			if (lua_isstring(myState, -2))
			{
				std::string key = lua_tostring(myState, -2);
				aTableMapOut[key] = SSArgument();

				switch (lua_type(myState, -1))
				{
				case LUA_TNUMBER:
					aTableMapOut[key] = SSArgument(lua_tonumber(myState, -1));
					break;
				case LUA_TSTRING:
					aTableMapOut[key] = SSArgument(luaL_tolstring(myState, -1, nullptr));
					break;
				case LUA_TBOOLEAN:
					aTableMapOut[key] = SSArgument(lua_toboolean(myState, -1) != 0);
					break;
				default:
					DL_ASSERT("Lua type is at the moment incompatible with Engine or at least this method");
					result = false;
					break;
				}
			}
			else
			{
				DL_ASSERT("Error in file %s, table %s: Lua key must be string", aScriptPath, aTableName);
				result = false;
			}

			lua_pop(myState, 2);
		}

		lua_pop(myState, 1);

		return result;
	}

	template<>
	void LuaWrapper::Pop<void>()
	{
		lua_pop(myState, 1);
	}

	bool LuaWrapper::DoCall(const int aArgumentCount, const int aReturnCount)
	{
		int result = lua_pcall(myState, aArgumentCount, aReturnCount, 0);
		return result == 0;
	}

	SSArgument LuaWrapper::CallLuaFunction(const std::string& aFunctionName, const ArgumentList& someArguments, const bool aShouldReturnFlag)
	{
		lua_getglobal(myState, aFunctionName.c_str());
		
		for (unsigned i = 0; i < someArguments.Size(); ++i)
		{
			const SSArgument& explainingArgument = someArguments[i];
			switch (explainingArgument.GetType())
			{
			case eSSType::NUMBER:
				lua_pushnumber(myState, explainingArgument.GetNumber());
				break;
			case eSSType::STRING: 
				lua_pushstring(myState, explainingArgument.GetString());
				break;
			case eSSType::BOOL: 
				lua_pushboolean(myState, explainingArgument.GetBool());
				break;
			case eSSType::NIL: break;
			default: break;
			}
		}

		int numberOfreturnValues = 0;
		if (aShouldReturnFlag == true)
		{
			numberOfreturnValues = 1;
		}

		const int result = lua_pcall(myState, someArguments.Size(), numberOfreturnValues, 0);

		if (result != NULL)
		{
			std::string errorString = "error running function " + aFunctionName + " : " + lua_tostring(myState, -1);
			ERROR_MESSAGE(stringToWstring(errorString).c_str());
		}

		if (numberOfreturnValues == 0)
		{
			return SSArgument();
		}

		switch (lua_type(myState, -1))
		{
		case LUA_TNUMBER:
			return  SSArgument(lua_tonumber(myState,-1));
		case LUA_TSTRING:
			return  SSArgument(luaL_tolstring(myState, -1, nullptr));
		case LUA_TBOOLEAN:
			return  SSArgument(lua_toboolean(myState, -1) != 0);
		case LUA_TNIL:
			break;
		default:
			DL_ASSERT("Lua type %s is at the moment incompatible with Engine or at least this method", lua_typename(myState, -1));
			break;
		}

		return SSArgument();
	}

	bool LuaWrapper::CheckArguments(const CU::DynamicString& aFunctionName, const CU::GrowingArray<eSSType>& aListOfTypes, const ArgumentList& anArgumentList)
	{
		if (aListOfTypes.Size() > anArgumentList.Size())
		{
			CU::DynamicString errorMsg = CU::DynamicString("ERROR! Lua C function ") + aFunctionName + " got " + anArgumentList.Size() + "arguments expected " + aListOfTypes.Size();
			errorMsg += " ( ";
			for (unsigned int i = 0; i < aListOfTypes.Size(); ++i)
			{
				if (i != 0)
				{
					errorMsg += " , ";
				}

				errorMsg += SSArgument::GetTypeName(aListOfTypes[i]);
			}
			errorMsg += " )";

			ERROR_MESSAGE(stringToWstring(errorMsg.c_str()).c_str());
			DL_ASSERT(errorMsg.c_str());
			return false;
		}

		for (unsigned int i = 0; i < aListOfTypes.Size(); ++i)
		{
			if (aListOfTypes[i] != anArgumentList[i].GetType())
			{
				CU::DynamicString errorMsg = CU::DynamicString("ERROR! Lua C function ") + aFunctionName + " got a " + anArgumentList[i].GetTypeName() +" as argument nr "+ i + 1 +" expected a " + SSArgument::GetTypeName(aListOfTypes[i]);
				ERROR_MESSAGE(stringToWstring(errorMsg.c_str()).c_str());
				DL_ASSERT(errorMsg.c_str());
				return false;
			}
		}
		return true;
	}

	void LuaWrapper::PrintDocumentation()
	{
		std::ofstream outPutFile("Script/Documentation/ExposedFunctions.txt");

		int index = 1;
		std::map<std::string, std::string>::iterator it;
		for (it = myExposedFunctions.begin(); it != myExposedFunctions.end(); it++)
		{
			outPutFile << index++ << ". " << it->first << ":\n" << it->second << std::endl << std::endl;
		}

		outPutFile.close();
	}

	LuaWrapper::LuaWrapper()
	{
		myState = luaL_newstate();
		luaL_openlibs(myState);

		lua_pushcfunction(myState, LuaPrint);
		lua_setglobal(myState, "print");
		//lua_pop(myState, 1);

	}

	LuaWrapper::~LuaWrapper()
	{
	}

	void LuaWrapper::CheckError(const int luaResult) const
	{
		CheckError(luaResult, myState);
	}

	void LuaWrapper::CheckError(const int luaResult, lua_State* aLuaState)
	{
		if (luaResult != LUA_OK)
		{
			std::string message = lua_tostring(aLuaState, -1);
			lua_pop(aLuaState, 1);

			size_t first = message.find("'");
			std::string sub1 = message.substr(first + 1);
			size_t second = sub1.find("'");
			std::string sub2 = sub1.substr(0, second);

			int difference = 99;
			int resultDifference = 0;
			std::string didYouMean;
			std::map<std::string, std::string>::iterator it;
			for (it = LuaWrapper::GetInstance().myExposedFunctions.begin(); it != LuaWrapper::GetInstance().myExposedFunctions.end(); it++)
			{
				resultDifference = LuaWrapper::GetInstance().YouCanTSpell(sub2, it->first);
				if (resultDifference < difference)
				{
					difference = resultDifference;
					didYouMean = it->first;
				}
			}

			std::cout << "You seem to have spelled" << sub2 << " did you mean " << didYouMean << "?" << std::endl;
			system("pause");
		}
	}


	

	size_t LuaWrapper::YouCanTSpell(const std::string & aString, const std::string & anotherString)
	{
		const size_t m(aString.size());
		const size_t n(anotherString.size());

		if (m == 0) return n;
		if (n == 0) return m;

		size_t *costs = new size_t[n + 1];

		for (size_t k = 0; k <= n; k++) costs[k] = k;

		size_t i = 0;
		for (std::string::const_iterator it1 = aString.begin(); it1 != aString.end(); ++it1, ++i)
		{
			costs[0] = i + 1;
			size_t corner = i;

			size_t j = 0;
			for (std::string::const_iterator it2 = anotherString.begin(); it2 != anotherString.end(); ++it2, ++j)
			{
				size_t upper = costs[j + 1];
				if (*it1 == *it2)
				{
					costs[j + 1] = corner;
				}
				else
				{
					size_t t(upper<corner ? upper : corner);
					costs[j + 1] = (costs[j]<t ? costs[j] : t) + 1;
				}

				corner = upper;
			}
		}

		size_t result = costs[n];
		delete[] costs;

		return result;
	}

	void LuaWrapper::UpdateFileWatcher()
	{
		myFileWatcher.FlushChanges(); //Needs to be done every frame
	}

	const std::map<std::string, LuaCallbackFunction>& LuaWrapper::GetExposedConsoleFunctions()
	{
		return myConsoleFunctions;
	}
	

	int LuaWrapper::StaticLuaVoidCallback(lua_State* aState)
	{
		return GetInstance().LuaVoidCallback(aState);
	}

	int LuaWrapper::LuaVoidCallback(lua_State* aState)
	{
		std::string functionName = lua_tostring(aState, lua_upvalueindex(1));

		ArgumentList argumentList = CreateArgumentList(aState);

		const ArgumentList returnArguments = myVoidFunctions[functionName](argumentList);

		for (unsigned int i = 0; i < returnArguments.Size(); i++)
		{
			switch (returnArguments[i].GetType())
			{
			case eSSType::NUMBER: 
				lua_pushnumber(aState, returnArguments[i].GetNumber());
				break;
			case eSSType::STRING:
				lua_pushstring(aState, returnArguments[i].GetString());
				break;
			case eSSType::BOOL:
				lua_pushboolean(aState, returnArguments[i].GetBool());
				break;
			case eSSType::LIGHTUSERDATA:
				lua_pushlightuserdata(aState, returnArguments[i].GetUserData());
				break;
			case eSSType::NIL:
				lua_pushnil(aState);
				break;
			default: break;
			}
		}

		return returnArguments.Size();
	}

	int LuaWrapper::LuaPrint(lua_State* aState)
	{
		int numberOfArguments = lua_gettop(aState);

		for (int i = 0; i < numberOfArguments; ++i)
		{
			if (lua_isstring(aState, i + 1))
			{
				const char * explainingString = lua_tostring(aState, i +1 );
#ifndef RESOURCES_LOG
				DL_PRINT(explainingString);
#else
				RESOURCES_LOG(explainingString);
#endif
			}
			else
			{
				MessageBeep(MB_ICONERROR);
				MessageBox(nullptr, L"lua Can only print strings at the moment. sorry :( use tostring or something", L"lua error", MB_OK | MB_ICONERROR);
				DL_ASSERT("lua Can only print strings at the moment. sorry :( use tostring or something");
			}
		}
		return 0;
	}

	void LuaWrapper::FileChanged(const std::wstring & aFileName)
	{
		lua_State* newState = luaL_newstate();
		luaL_openlibs(newState);

		//CheckErrors(luaL_loadfile(myState, aFileName.c_str()),myState);
		const wchar_t* help = aFileName.c_str();
		char* helpMePlease;
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
		//std::wcstombs(helpMePlease, help, 100);
		CheckError(luaL_dofile(newState, convert.to_bytes(aFileName).c_str()), newState);

		lua_close(myState);

		myState = newState;

		myExposedFunctions.clear();

		lua_pushcfunction(myState, LuaPrint);
		lua_setglobal(myState, "print");

		myExposedFunctions["print"] = "Prints text in console";
		

		std::map<std::string, SSlua::LuaCallbackFunction>::iterator it;
		for (it = LuaWrapper::GetInstance().myVoidFunctions.begin(); it != LuaWrapper::GetInstance().myVoidFunctions.end(); it++)
		{
			RegisterFunction(it->second, it->first, "", false);
		}
	}

	ssLuaTable LuaWrapper::GetTable(lua_State* aState, int aIndex)
	{
		ssLuaTable tempLuaTable;
		lua_pushnil(aState);
		while (lua_next(aState, aIndex) != 0)
		{
			SSArgument tempArgument;
			switch (lua_type(aState, -1))
			{
			case LUA_TNUMBER:
				tempArgument = SSArgument(lua_tonumber(aState, -1));
				break;
			case LUA_TSTRING:
				tempArgument = SSArgument(luaL_tolstring(aState, -1, nullptr));
				break;
			case LUA_TBOOLEAN:
				tempArgument = SSArgument(lua_toboolean(aState, -1) != 0);
				break;
			case LUA_TNIL:
				break;
			case LUA_TTABLE:
				tempArgument = SSArgument(GetTable(aState, -1));
				break;
			default:
				DL_ASSERT("Lua type is at the moment incompatible with Engine or at least this method");
			}
			if (lua_isstring(aState, -2))
			{
				tempLuaTable[lua_tolstring(aState, -2, nullptr)] = tempArgument;
			}
			else if (lua_isnumber(aState, -2))
			{
				std::string key;
				key += std::to_string(lua_tointeger(aState, -2));
				tempLuaTable[key] = tempArgument;
			}
			lua_pop(aState, 1);
		}

		return tempLuaTable;
	}

	ArgumentList LuaWrapper::CreateArgumentList(lua_State* aState)
	{
		ArgumentList arguments;
		arguments.Init(1);

		const int nArgs = lua_gettop(aState);

		if (nArgs == 0)
		{
			return std::move(arguments);
		}

		for (int i = 1; i < nArgs + 1; i++)
		{
			SSArgument tempArgument;
			switch (lua_type(aState, i))
			{
			case LUA_TNUMBER:
				tempArgument = SSArgument(lua_tonumber(aState, i));
				break;
			case LUA_TSTRING:
				tempArgument = SSArgument(luaL_tolstring(aState, i, nullptr));
				break;
			case LUA_TBOOLEAN:
				tempArgument = SSArgument(lua_toboolean(aState, i) != 0);
				break;
			case LUA_TLIGHTUSERDATA:
				tempArgument = SSArgument(lua_touserdata(aState, i));
				break;
			case LUA_TNIL:
				break;
			case LUA_TTABLE:
				tempArgument = SSArgument(GetTable(aState, i));
				break;
			default:
				DL_ASSERT("Lua type is at the moment incompatible with Engine or at least this method");
			}
			arguments.Add(tempArgument);
		}

		return arguments;
	}
}
