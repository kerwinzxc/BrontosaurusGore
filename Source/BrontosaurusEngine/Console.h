#pragma once
#include <map>
#include "../PostMaster/Subscriber.h"

namespace CU
{
	class DynamicString;
	enum class eKeys;
}

struct SSArgument;

namespace SSlua
{
	typedef CU::GrowingArray<SSArgument> ArgumentList;
	typedef std::function<ArgumentList(const ArgumentList&)> LuaCallbackFunction;
}

class CTextInstance;

class CConsole : public Subscriber
{
public:
	CConsole();
	~CConsole();

	void Init();
	void GetLuaFunctions();
	void Activate();
	void Deactivate();
	eMessageReturn Recieve(const Message & aMessage) override;
	eMessageReturn TakeKeyBoardInputPressedChar(const char aKey);
	eMessageReturn TakeKeyBoardInput(const CU::eKeys aKey);
	

	bool Update(float aDeltaTime);
	void Render();

private:
	void UpdateCommandSuggestions(const std::string& aStringToCompare);
	size_t CConsole::MakeCommandSuggestions(const std::string& aStringToCompare, const std::string& aStringToEvaluate);
	const CU::DynamicString CheckIfTextIsCommand(const CU::DynamicString& aText);
	void Print(const CU::DynamicString& aText);
	void PrintCommands();

	CU::DynamicString ParseAndRunFunction(const CU::DynamicString& aString);

private:
	std::map<std::string, SSlua::LuaCallbackFunction> myLuaFunctions;
	CU::GrowingArray<CTextInstance*> myTextLog;
	CTextInstance* myCurrentText;
	CTextInstance* mySuggestedCommand;
	//std::string mySuggestedCommand;

	float myElapsedAnimationTimer;
	float myAnimationTimerCooldown;
	bool myHaveIAfterCurrentText;
	bool myIsActive;
};

