#include "stdafx.h"
#include "StringHelper.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

//#include <stdlib.h> // mbstowcs_s
//#include <string.h> // strlen

namespace CU
{
	wchar_t* CharToWChar(wchar_t aBuffer[], const char* aText)
	{
		size_t size = strlen(aText) + 1;
		size_t outSize;
#ifdef _WIN32
		// retval , dst, dstsz, scr, len
		mbstowcs_s(&outSize, aBuffer, size, aText, size - 1);
#else
		// dst, src, dstsz or len
		outSize = mbstowcs(aBuffer, aText, size - 1);
		outSize;
#endif
		return aBuffer;
	}

	std::wstring StringToWString(const std::string& aString)
	{
		return std::wstring(aString.begin(), aString.end());
	}
	std::string& FindAndErase(std::string& aStringToClean, const std::string& aSubStringToErase)
	{
		size_t pos = aStringToClean.find(aSubStringToErase);
		if (pos != std::string::npos)
		{
			aStringToClean.erase(pos, aSubStringToErase.size());
		}

		return aStringToClean;
	}

	std::string& FindAndReplace(std::string& aStringToClean, const std::string& aSubStringToErase, const std::string& aSubStringToReplaceWith)
	{
		size_t pos = aStringToClean.find(aSubStringToErase);
		if (pos != std::string::npos)
		{
			aStringToClean.erase(pos, aSubStringToErase.size());
			aStringToClean.insert(pos, aSubStringToReplaceWith);
		}

		return aStringToClean;
	}

	std::string& ToLowerCase(std::string& aString)
	{
		std::transform(aString.begin(), aString.end(), aString.begin(), ::tolower);
		return aString;
	}

	std::string& ToUpperCase(std::string& aString)
	{
		std::transform(aString.begin(), aString.end(), aString.begin(), ::toupper);
		return aString;
	}

	std::wstring& ToLowerCase(std::wstring& aString)
	{
		std::transform(aString.begin(), aString.end(), aString.begin(), ::towlower);
		return aString;
	}

	std::wstring& ToUpperCase(std::wstring& aString)
	{
		std::transform(aString.begin(), aString.end(), aString.begin(), ::towupper);
		return aString;
	}

	bool StringHelper::IsBool(const std::string& aString)
	{
		return aString == "true" || aString == "false" || aString == "True" || aString == "False" || aString == "TRUE" || aString == "FALSE";
	}

	bool StringHelper::ToBool(const std::string& aString)
	{
		if (aString == "true" || aString == "True" || aString == "TRUE")
		{
			return true;
		}
		if(aString == "false" || aString == "False" || aString == "FALSE")
		{
			return false;
		}
		DL_ASSERT("Could not convert");
		return false;
	}

	bool StringHelper::IsFloat(const std::string& aString)
	{
		const std::string number = "1234567890.";
		for (int i = 0; i < aString.size(); ++i)
		{
			if (number.find(aString[i]) == std::string::npos)
			{
				return false;
			}
		}

		return true;
	}

	bool StringHelper::IsInt(const std::string& aString)
	{
		const std::string number = "-1234567890";
		for (int i = 0; i < aString.size(); ++i)
		{
			if (number.find(aString[i]) == std::string::npos)
			{
				return false;
			}
		}

		return true;
	}

	std::string StringHelper::ToStringWithPrecision(const float aFloat, const char aPrecision)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(aPrecision) << aFloat;
		return ss.str();
	}

	std::wstring StringHelper::ToWStringWithPrecision(const float aFloat, const char aPrecision)
	{
		std::wstringstream ss;
		ss << std::fixed << std::setprecision(aPrecision) << aFloat;
		return ss.str();
	}

	std::string StringHelper::ToStringWithPrecision(const double aDouble, const char aPrecision)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(aPrecision) << aDouble;
		return ss.str();
	}

	std::string StringHelper::WStringToString(const std::wstring aWString)
	{
		return std::string(aWString.begin(), aWString.end());
	}
}

std::string& operator-=(std::string& aLeft, const std::string& aRight)
{
	size_t pos = aLeft.rfind(aRight);
	if (pos != std::string::npos)
	{
		aLeft.erase(aLeft.end() - aRight.length(), aLeft.end());
	}

	return aLeft;
}

std::wstring& operator-=(std::wstring& aLeft, const std::wstring& aRight)
{
	size_t pos = aLeft.rfind(aRight);
	if (pos != std::string::npos)
	{
		aLeft.erase(aLeft.end() - aRight.length(), aLeft.end());
	}

	return aLeft;
}

std::string& operator^=(std::string& aLeft, const std::string& aRight)
{
	return CU::FindAndErase(aLeft, aRight);
}
