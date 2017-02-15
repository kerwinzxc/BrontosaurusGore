#pragma once
#include <map>

class CommandLineManager
{
public:
	static void CreateInstance(int argc, char* argv[]);
	static void DestroyInstance();
	static CommandLineManager* GetInstance();
	
	bool HasParameter(const char* aParameter);
	bool HasArgument(const char* aParameter, const char* aArgument);
	const std::string& GetArgument(const char* aParameter);

private:
	CommandLineManager(int argc, char* argv[]);
	~CommandLineManager();

	std::map<std::string, CU::GrowingArray<std::string>> myArgs;

	static CommandLineManager* myInstance;
	static const std::string ourEmptyString;
};

