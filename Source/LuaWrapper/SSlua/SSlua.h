// 
// 2016-07-08
#pragma once
#include <functional>

#include <map>
#include "../SSArgument/SSArgument.h"

#include "../file_watcher.h"

#define LUA_WRAPPER SSlua::LuaWrapper::GetInstance()

struct lua_State;
struct SSArgument;

namespace SSlua
{
	typedef CU::GrowingArray<SSArgument> ArgumentList;
	typedef CU::GrowingArray<eSSType> TypeList;
	typedef std::function<ArgumentList(const ArgumentList&)> LuaCallbackFunction;

	class LuaWrapper
	{
	public:
		static void DestroyIfCreated();
		static LuaWrapper& GetInstance();

		void RegisterFunctions(const std::function<void(LuaWrapper&)>& aRegisterFunctionsFunction);
		void RegisterFunction(const LuaCallbackFunction &aFunction, const std::string &aName,const std::string& aHelpText, const bool aShouldBeExposedToConsole);
		void RegisterEngineFunction(const LuaCallbackFunction &aFunction, const std::string &aName);

		void AddScriptPath(std::string aPath);

		void LoadCode(const std::string& aFileName);
		void RunLoadedCode();
		bool DoFile(const std::string& aFileName);
		bool DoString(const std::string& aPieceOfCode);
		bool LoadLuaString(const std::string& aLuaScript);
		void GetGlobal(const std::string& aVariableName, int* const aType = nullptr);
		void AssignePairToTableAt(const int aTableIndex);
		void GetValueInTableAt(const int aTableIndex);
		void GetValueFromTable(const std::string& aTableName, const std::string& aKeyName);
		void GetLastError(std::string& aErrorMessageOut) const;

		template<typename T>
		void Push(const T& aVariable);

		template<typename T>
		T Pop();

		bool DoCall(const int aArgumentCount, const int aReturnCount);

		SSArgument CallLuaFunction(const std::string& aFunctionName, const ArgumentList& someArguments, const bool aShouldReturnFlag = false);

		static bool CheckArguments(const std::string &aFunctionName,const CU::GrowingArray<eSSType> &aListOfTypes,const ArgumentList &anArgumentList); 

		//Alex
		void PrintDocumentation();
		size_t YouCanTSpell(const std::string& aString, const std::string& anotherString);

		void UpdateFileWatcher();

		const std::map<std::string, LuaCallbackFunction>& GetExposedConsoleFunctions();
		//

	private:
		LuaWrapper();
		~LuaWrapper();

		std::string myEngineTableName = "Engine";

		static void CheckAndCreate();
		static bool CheckCreated();

		void CheckError(const int luaResult) const;
		static void CheckError(const int luaResult, lua_State* aLuaState);

		static int StaticLuaVoidCallback(lua_State* aState);
		int LuaVoidCallback(lua_State* aState);

		static int LuaPrint(lua_State* aState);

		//Alex
		void FileChanged(const std::wstring & aFileName); //Work In progress
		//

		static ssLuaTable GetTable(lua_State* aState, int aIndex);
		ArgumentList CreateArgumentList(lua_State* aState);

		static LuaWrapper* ourInstance;

		lua_State* myState;

		std::map<std::string, LuaCallbackFunction> myVoidFunctions;

		//Alex
		std::map < std::string, std::string> myExposedFunctions;
		std::map < std::string, LuaCallbackFunction> myConsoleFunctions;

		LuaCallbackFunction myRegisterFunctionsFunction;
		CFileWatcher myFileWatcher;
		std::string myScriptPath;
		//
	};

}
