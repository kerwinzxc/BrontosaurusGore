#pragma once
#include <map>
#include "../PostMaster/Subscriber.h"
#include "../ThreadedPostmaster/Subscriber.h"

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

class CConsole : public Postmaster::ISubscriber
{
public:
	CConsole();
	~CConsole();

	void Init();
	void GetLuaFunctions();
	void Activate();
	void Deactivate();
	eMessageReturn TakeKeyBoardInputPressedChar(const char aKey);
	eMessageReturn TakeKeyBoardInput(const CU::eKeys aKey);
	

	bool Update(float aDeltaTime);
	void Render();

	eMessageReturn DoEvent(const KeyCharPressed& aCharPressed) override;
private:
	void UpdateCommandSuggestions(const std::string& aStringToCompare);
	size_t CConsole::MakeCommandSuggestions(const std::string& aStringToCompare, const std::string& aStringToEvaluate);
	const std::string CheckIfTextIsCommand(const std::string& aText);
	void Print(const std::string& aText);
	void PrintCommands();

	std::string ParseAndRunFunction(const std::string& aString);


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

