#pragma once
#include <vector>

//FORWARD DECLERATION BEGIN
//std::string and std::wstring

template<class _Elem, class _Traits, class _Alloc>
class basic_string;

template<class _Elem>
struct char_traits;

template<class _Ty>
class allocator;

typedef basic_string<char, char_traits<char>, allocator<char>> string;
typedef basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t>> wstring;

//FORWARD DECLERATION END

namespace CU
{
	wchar_t* CharToWChar(wchar_t aBuffer[], const char* aText);
	std::wstring StringToWString(const std::string& aString);
	std::string& FindAndErase(std::string& aStringToClean, const std::string& aSubStringToErase);
	std::string& FindAndReplace(std::string& aStringToClean, const std::string& aSubStringToErase, const std::string& aSubStringToReplaceWith);
	std::string& ToLowerCase(std::string& aString);
	std::string& ToUpperCase(std::string& aString);
	std::wstring& ToLowerCase(std::wstring& aString);
	std::wstring& ToUpperCase(std::wstring& aString);

	namespace StringHelper
	{
		bool IsBool(const std::string& aString);
		bool ToBool(const std::string& aString);

		bool IsFloat(const std::string& aString);
		bool IsInt(const std::string& aString);

		std::string ToStringWithPrecision(const float aFloat, const char aPrecision = 3);
		std::wstring ToWStringWithPrecision(const float aFloat, const char aPrecision = 3);

		std::string ToStringWithPrecision(const double aDouble, const char aPrecision = 3);

		std::string WStringToString(const std::wstring aWString);

		std::vector<std::string> Split(const std::string& aStringToSplit, const std::string& aSplitValues);
	}
}

std::string& operator-=(std::string& aLeft, const std::string& aRight);
std::wstring& operator-=(std::wstring& aLeft, const std::wstring& aRight);
std::string& operator^=(std::string& aLeft, const std::string& aRight);