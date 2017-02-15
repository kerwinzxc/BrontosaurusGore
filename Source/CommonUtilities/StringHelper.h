#pragma once

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
}

std::string& operator-=(std::string& aLeft, const std::string& aRight);
std::wstring& operator-=(std::wstring& aLeft, const std::wstring& aRight);
std::string& operator^=(std::string& aLeft, const std::string& aRight);