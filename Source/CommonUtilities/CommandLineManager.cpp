#include "stdafx.h"
#include "CommandLineManager.h"

CommandLineManager* CommandLineManager::myInstance = nullptr;
const std::string CommandLineManager::ourEmptyString = "";

void CommandLineManager::CreateInstance(int argc, char * argv[])
{
	assert(myInstance == nullptr && "Command line parser should not be created twice");
	myInstance = new CommandLineManager(argc, argv);
}

CommandLineManager* CommandLineManager::GetInstance()
{
	return myInstance;
}

void CommandLineManager::DestroyInstance()
{
	assert(myInstance != nullptr && "Command line parser not created (is NULL)");
	SAFE_DELETE(myInstance);
}

bool CommandLineManager::HasParameter(const char* aParameter)
{
	return myArgs.find(aParameter) != myArgs.end();
	//for (unsigned int i = 0; i < myArgs.Size(); ++i)
	//{
	//	if (myArgs[i] == aParameter)
	//	{
	//		return true;
	//	}
	//}
	//return false;
}

const std::string& CommandLineManager::GetArgument(const char* aParameter)
{
	if (HasParameter(aParameter) == true)
	{
		return myArgs[aParameter][0]; //fix this where it's used
	}

	return ourEmptyString;

	//for (unsigned int i = 0; i < myArgs.Size(); ++i)
	//{
	//	if (myArgs[i] == aParameter)
	//	{
	//		for (unsigned int j = i + 1; j < myArgs.Size(); ++j)
	//		{
	//			return myArgs[j];
	//		}
	//	}
	//}
	//return false;
}

bool CommandLineManager::HasArgument(const char* aParameter, const char* aArgument)
{
	if (HasParameter(aParameter) == true)
	{
		if (myArgs[aParameter].Find(aArgument) != myArgs[aParameter].FoundNone)
		{
			return true;
		}
	}

	return false;

	//for (unsigned int i = 0; i < myArgs.Size(); ++i)
	//{
	//	if (myArgs[i] == aParameter)
	//	{
	//		for (unsigned int j = i + 1; j < myArgs.Size(); ++j)
	//		{
	//			if (myArgs[j] == aArgument)
	//			{
	//				return true;
	//			}
	//			else if (myArgs[j][0] == '-')
	//			{
	//				return false;
	//			}
	//		}
	//	}
	//}
	//return false;
}

CommandLineManager::CommandLineManager(int aArgc, char* aArgv[])
{
	for (int i = 0; i < aArgc; ++i)
	{
		if (aArgv[i][0] == '-')
		{
			const char* argument = aArgv[i];
			while (i + 1 < aArgc && aArgv[i + 1][0] != '-')
			{
				if (myArgs[argument].IsInitialized() == false)
				{
					myArgs[argument].Init(4);
				}

				myArgs[argument].Add(aArgv[++i]);
			}
		}
	}

	for (auto it = myArgs.begin(); it != myArgs.end(); ++it)
	{
		it->second.ShrinkToFit();
	}
}

CommandLineManager::~CommandLineManager()
{
}
