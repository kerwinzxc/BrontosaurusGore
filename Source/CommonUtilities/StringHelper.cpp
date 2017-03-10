#include "stdafx.h"
#include "StringHelper.h"
#include <sstream>
#include <iomanip>

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

	bool StringHelper::IsBool(std::string aString)
	{
		return aString == "true" || aString == "false" || aString == "True" || aString == "False" || aString == "TRUE" || aString == "FALSE";
	}

	bool StringHelper::ToBool(std::string astring)
	{
		if (astring == "true" || astring == "True" || astring == "TRUE")
		{
			return true;
		}
		if(astring == "false" || astring == "False" || astring == "FALSE")
		{
			return false;
		}
		DL_ASSERT("Could not convert");
		return false;
	}

	bool StringHelper::IsFloat(std::string aString)
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

	bool StringHelper::IsInt(std::string aString)
	{
		const std::string number = "1234567890";
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

	std::string StringHelper::ToStringWithPrecision(const double aDouble, const char aPrecision)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(aPrecision) << aDouble;
		return ss.str();
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
