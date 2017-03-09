#pragma once

struct SSArgument;
enum class eSSType;

namespace SSlua
{
	typedef CU::GrowingArray<SSArgument> ArgumentList;
	typedef CU::GrowingArray<eSSType> TypeList;
}

namespace ScriptHelper
{
	bool CheckArgumentCount(const std::string& aFunctionName, const unsigned int aPreferred, const unsigned int aRecieved, const bool aForceEquality);
	bool CheckArgumentList(const std::string& aFunctionName, const SSlua::TypeList& aPreferred, const SSlua::ArgumentList& aRecieved, const bool aForceEquality);

	bool AssertArgumentCount(const std::string& aFunctionName, const unsigned int aPreferred, const unsigned int aActual, const bool aForceEquality);
	bool AssertArgumentList(const std::string& aFunctionName, const SSlua::TypeList& aPreferred, const SSlua::ArgumentList& aRecieved, const bool aForceEquality);
}
