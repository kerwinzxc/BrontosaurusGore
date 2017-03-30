#pragma once
#include "../CommonUtilities/StaticArray.h"

#define ENUM_STRING_MACRO(name, ...)											\
enum class e##name { __VA_ARGS__, eLength };									\
constexpr int name##hiddenlength = static_cast<int>(e##name::eLength);			\
using name##hiddentypedef = SAnimationState<name##hiddenlength>;				\
static name##hiddentypedef locHiddenAnimationState##name(#__VA_ARGS__);

template<int NumStates>
struct SAnimationState 
{
	SAnimationState(const char* aCommaSeperatedString);
	static CU::StaticArray<std::string, NumStates> AnimationStates;
};

template <int NumStates>
CU::StaticArray<std::string, NumStates> SAnimationState<NumStates>::AnimationStates;

template<int NumStates>
inline SAnimationState<NumStates>::SAnimationState(const char* aCommaSeperatedString)
{
	std::string subString = aCommaSeperatedString;
	for (int i = 0; i < NumStates; ++i)
	{
		size_t nextComma = subString.find(',');
		if (nextComma == std::string::npos)
		{
			AnimationStates[i] = subString;
			break;
		}

		AnimationStates[i] = subString.substr(0, nextComma);
		subString = subString.substr(nextComma + 2);
	}
}
