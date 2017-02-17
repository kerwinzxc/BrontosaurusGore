#include "stdafx.h"
#include <stdlib.h>
#include "DL_Debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <ctime>
#include <iomanip>
#include "DL_Assert.h"
#include <iostream>
#include <fstream>

#include "StringHelper.h"
#include "CommandLineManager.h"

#pragma warning(disable : 4091)
#include "DL_StackWalker.h"
#pragma warning(default : 4091)

#ifdef __APPLE__
#define VSPRINTF vsprintf
#elif defined _WIN32 || defined _WIN64
#define VSPRINTF vsprintf_s
#else
#error "Is this debian?"
#endif

#define SUPRESS_UNUSED_WARNING(variable) variable
const unsigned int MAX_STRING_BUFFER_SIZE = 1024u;

namespace DL_Debug
{
	Debug* Debug::ourInstance = nullptr;
	
	class Log
	{
	public:
		inline void CreateLog(const char* aFileName);
		inline void Write(const char* aMessage);
		inline void Activate() { myIsActive = true; }
		inline void Deactivate() { myIsActive = false; }

	private:
		std::ofstream myDebugFile;
		std::string myFile;
		bool myIsActive;
	};

	void Log::CreateLog(const char* aFilePath)
	{
		myIsActive = false;
		myFile = aFilePath;
	}

	void Log::Write(const char* aMessage)
	{
		if (myIsActive == false)
		{
			return;
		}

		myDebugFile.open(myFile, std::ios::app);

		time_t rawTime = time(nullptr);
		tm timeInfo = {};
		char timebuffer[80];
		
		localtime_s(&timeInfo, &rawTime);
		strftime(timebuffer, 80, "%d-%m-%Y %H:%M:%S", &timeInfo);

		myDebugFile << timebuffer << "	" << aMessage << std::endl;

		myDebugFile.close();
	}

	Debug::Debug()
		: myLogFiles(nullptr)
	{
		ActivateLogs();
	}

	Debug::~Debug()
	{
		myLogFiles.DeleteAll();
	}

	void Debug::CreateLog(eLogTypes aLogType)
	{
		int logType = static_cast<int>(aLogType);
		if (myLogFiles[logType] != nullptr)
		{
			assert(!"Logtype already created");
			return;
		}

		myLogFiles[logType] = new Log();
		std::string fileName;
		const char* suffix = ".loggo";
		switch (aLogType)
		{
		case DL_Debug::eLogTypes::eEngine:
			fileName = "LOG_ENGINE";
			break;
		case DL_Debug::eLogTypes::eGamePlay:
			fileName = "LOG_GAMEPLAY";
			break;
		case DL_Debug::eLogTypes::eResource:
			fileName = "LOG_RESOURCES";
			break;
		case DL_Debug::eLogTypes::eCrash:
			fileName = "LOG_CRASH";
			break;
		case DL_Debug::eLogTypes::eThreadedModels:
			fileName = "LOG_THREADED_MODELS";
			break;
		case DL_Debug::eLogTypes::eThreadPool:
			fileName = "LOG_THREADPOOL";
			break;

		case DL_Debug::eLogTypes::ePhysics:
			fileName = "LOG_PHYSICS";
			break;
		default:
			fileName = "LOG_DEFAULT";
			break;
		}
		fileName += suffix;
		myLogFiles[logType]->CreateLog(fileName.c_str());
	}

	void Debug::CreateInstance()
	{
		assert(ourInstance == nullptr && "Debug logger already created");
		ourInstance = new Debug();
	}

	void Debug::DestroyInstance()
	{
		assert(ourInstance != nullptr && "Debug logger not created (is NULL)");
		SAFE_DELETE(ourInstance);
	}

	Debug* Debug::GetInstance()
	{
		assert(ourInstance != nullptr && "Debug logger not created (is NULL)");
		return ourInstance;
	}

	void Debug::ActivateLogs()
	{
		CommandLineManager* commandLineManager = CommandLineManager::GetInstance();
		if (commandLineManager != nullptr)
		{
			if (commandLineManager->HasArgument("-log", "gameplay"))
			{
				Activate(DL_Debug::eLogTypes::eGamePlay);
			}
			if (commandLineManager->HasArgument("-log", "resource"))
			{
				Activate(DL_Debug::eLogTypes::eResource);
			}
			if (commandLineManager->HasArgument("-log", "engine"))
			{
				Activate(DL_Debug::eLogTypes::eEngine);
			}
			if (commandLineManager->HasArgument("-log", "crash"))
			{
				Activate(DL_Debug::eLogTypes::eCrash);
			}
			if (commandLineManager->HasArgument("-log", "threadedModels"))
			{
				Activate(DL_Debug::eLogTypes::eThreadedModels);
			}
			if (commandLineManager->HasArgument("-log", "threadPool"))
			{
				Activate(DL_Debug::eLogTypes::eThreadPool);
			}
			if (commandLineManager->HasArgument("-log", "physics"))
			{
				Activate(DL_Debug::eLogTypes::ePhysics);
			}
		}
	}

	void Debug::Activate(const eLogTypes aLogType)
	{
		int logType = static_cast<int>(aLogType);
		if (myLogFiles[logType] == nullptr)
		{
			CreateLog(aLogType);
		}
		
		myLogFiles[logType]->Activate();
	}

	void Debug::Deactivate(const eLogTypes aLogType)
	{
		int logType = static_cast<int>(aLogType);
		if (myLogFiles[logType] != nullptr)
		{
			myLogFiles[logType]->Deactivate();
		}
	}

	void Debug::AssertMessage(const char *aFileName, const int aLine, const char* /*aFunctionName*/, const char *aString, ...)
	{
		char buffer[MAX_STRING_BUFFER_SIZE] = {};
		va_list args;

		va_start(args, aString);
		VSPRINTF(buffer, aString, args);
		va_end(args);

#ifdef _WIN32
		StackWalker myStackWalker;
		//myDebugFile << myStackWalker.ShowCallstack() << std::endl << std::endl;
		wchar_t fileNameBuffer[MAX_STRING_BUFFER_SIZE];
		wchar_t bufferBuffer[MAX_STRING_BUFFER_SIZE];
		_wassert(CU::CharToWChar(bufferBuffer, buffer), CU::CharToWChar(fileNameBuffer, aFileName), aLine);
#else
		assert(false && charToWChar(aString) && charToWChar(aFileName) && aLine);
#endif
	}

	void Debug::AssertMessage(const char* aFileName, const int aLine, const char* /*aFunctionName*/, const wchar_t* aString, ...)
	{
		wchar_t buffer[MAX_STRING_BUFFER_SIZE] = {};
		va_list args;
		va_start(args, aString);
		wvsprintf(buffer, aString, args);
		va_end(args);

#ifdef _WIN32
		StackWalker myStackWalker;
		//myDebugFile << myStackWalker.ShowCallstack() << std::endl << std::endl;
		wchar_t fileNameBuffer[MAX_STRING_BUFFER_SIZE];
		_wassert(buffer, CU::CharToWChar(fileNameBuffer, aFileName), aLine);
#else
		assert(false && buffer && charToWChar(aFileName) && aLine);
#endif
	}

	void Debug::PrintMessage(const char *aFormattedString, ...)
	{
		char buffer[MAX_STRING_BUFFER_SIZE] = {};
		va_list args;

		va_start(args, aFormattedString);
		VSPRINTF(buffer, aFormattedString, args);
		va_end(args);

		std::cout << buffer << std::endl;
	}

	void Debug::PrintMessage(const wchar_t * aWString, ...)
	{
		wchar_t buffer[MAX_STRING_BUFFER_SIZE] = {};
		va_list args;
		va_start(args, aWString);
		wvsprintf(buffer, aWString, args);
		va_end(args);

		std::wcout << buffer << std::endl;
	}

	void Debug::WriteLog(const eLogTypes aLogType, const char* aFormattedString, ...)
	{
		Log* log = myLogFiles[static_cast<int>(aLogType)];
		if (log == nullptr)
		{
			return;
		}

		char buffer[MAX_STRING_BUFFER_SIZE] = {};
		va_list args;

		va_start(args, aFormattedString);
		VSPRINTF(buffer, aFormattedString, args);
		va_end(args);

		log->Write(buffer);
	}

	void Debug::ShowMessageBox(const char* aMessage, ...)
	{
#ifdef _WIN32
		char buffer[MAX_STRING_BUFFER_SIZE] = {};
		va_list args;

		va_start(args, aMessage);
		VSPRINTF(buffer, aMessage, args);
		va_end(args);

		int returnValue = MessageBoxA(nullptr, buffer, "Error (press 'abort' to exit program, 'retry' to debug, 'ignore' to continue", MB_ABORTRETRYIGNORE);

		if (returnValue == IDABORT)
		{
			exit(1);
		}
		else if (returnValue == IDRETRY)
		{
			DL_ASSERT(buffer);
		}
		else if (returnValue == IDIGNORE)
		{
			return;
		}
		//if (returnValue == IDCANCEL)
		//{
		//	assert(false); // TODO: FIX MINIDUMP AND SHIT
		//	exit(1);
		//}
#else
		aMessage;
		//implement mbx for unix
#endif
	}

	void Debug::ShowMessageBox(const wchar_t* aMessage, ...)
	{
#ifdef _WIN32
		wchar_t buffer[MAX_STRING_BUFFER_SIZE] = {};
		va_list args;

		va_start(args, aMessage);
		wvsprintf(buffer, aMessage, args);
		va_end(args);

		int returnValue = MessageBoxW(nullptr, buffer, L"Error (press 'abort' to exit program, 'retry' to debug, 'ignore' to continue", MB_ABORTRETRYIGNORE);

		if (returnValue == IDABORT)
		{
			exit(1);
		}
		else if (returnValue == IDRETRY)
		{
			DL_ASSERT(buffer);
		}
		else if (returnValue == IDIGNORE)
		{
			return;
		}

		//int returnValue = MessageBoxW(nullptr, buffer, L"Error (press cancel to quit, press retry to try to continue", MB_RETRYCANCEL);
		//if (returnValue == IDCANCEL)
		//{
		//	assert(false); // TODO: FIX MINIDUMP AND SHIT
		//	exit(1);
		//}
#else
		aMessage;
		//implement mbx for unix
#endif
	}

	void Debug::SetConsoleColor(const unsigned short aColor)
	{
#ifdef _WIN32
		HANDLE hConsole = nullptr;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hConsole != nullptr)
		{
			SetConsoleTextAttribute(hConsole, aColor);
		}
#else
		SUPRESS_UNUSED_WARNING(aColor);
#endif
	}

	wchar_t * Debug::ParseVariadicArgs(wchar_t aBuffer[], const char* aFormattedString, ...)
	{
		char buffer[MAX_STRING_BUFFER_SIZE] = {};
		va_list args;
		va_start(args, aFormattedString);
		VSPRINTF(buffer, aFormattedString, args);
		va_end(args);

		std::string charStr = buffer;
		std::wstring wcharStr(charStr.begin(), charStr.end());

		(void)memcpy_s(aBuffer, MAX_STRING_BUFFER_SIZE, wcharStr.c_str(), MAX_STRING_BUFFER_SIZE);

		return aBuffer;
	}

	wchar_t* Debug::ParseVariadicArgs(wchar_t aBuffer[], const wchar_t * aFormattedString, ...)
	{
		wchar_t buffer[MAX_STRING_BUFFER_SIZE] = {};
		va_list args;
		va_start(args, aFormattedString);
		wvsprintf(buffer, aFormattedString, args);
		va_end(args);

		(void)memcpy_s(aBuffer, MAX_STRING_BUFFER_SIZE, buffer, MAX_STRING_BUFFER_SIZE);

		return aBuffer;
	}
}