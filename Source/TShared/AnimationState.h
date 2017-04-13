#pragma once
#include "../CommonUtilities/StaticArray.h"

#define ENUM_STRING_MACRO(name, ...)											\
enum class e##name { __VA_ARGS__, eLength };									\
constexpr int name##hiddenlength = static_cast<int>(e##name::eLength);			\
using S##name = TAnimationState<name##hiddenlength>;							\
static S##name locHiddenAnimationState##name(#__VA_ARGS__);

template<int NumStates>
struct TAnimationState
{
	TAnimationState(const char* aCommaSeperatedString);
	static CU::StaticArray<std::string, NumStates> AnimationStates;
};

template <int NumStates>
CU::StaticArray<std::string, NumStates> TAnimationState<NumStates>::AnimationStates;

template<int NumStates>
inline TAnimationState<NumStates>::TAnimationState(const char* aCommaSeperatedString)
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

#define DECLARE_ANIMATION_ENUM_AND_STRINGS \
ENUM_STRING_MACRO(AnimationState, idle01, walk01, run01, shot01, equip01, unequip01, jump01, death01, meleeAttack01, throwAttack01, jumpLift01, jumpLanding01, jumpAttackOnly01, chargeRun01, chargeStart01, invisible, none)
