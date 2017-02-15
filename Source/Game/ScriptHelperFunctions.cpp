#include "stdafx.h"
#include "ScriptHelperFunctions.h"
#include "../LuaWrapper/SSArgument/SSArgument.h"

#define SCRIPT_HELPER_ERROR DL_MESSAGE_BOX

namespace ScriptHelper
{
	bool AssertArgumentCount(const std::string& aFunctionName, const unsigned int aPreferred, const unsigned int aRecieved, const bool aForceEquality)
	{
		if (aRecieved < aPreferred || (aRecieved != aPreferred && aForceEquality))
		{
			SCRIPT_HELPER_ERROR("Error in %s, expected %u argument, got %u", aFunctionName.c_str(), aPreferred, aRecieved);
			return false;
		}

		return true;
	}

	bool AssertArgumentList(const std::string& aFunctionName, const SSlua::TypeList& aPreferred, const SSlua::ArgumentList& aRecieved, const bool aForceEquality)
	{
		if (!AssertArgumentCount(aFunctionName, aPreferred.Size(), aRecieved.Size(), aForceEquality))
		{
			return false;
		}

		bool success = true;

		for (unsigned int i = 0; i < aPreferred.Size(); ++i)
		{
			if (aPreferred[i] != aRecieved[i].GetType())
			{
				SCRIPT_HELPER_ERROR("Argument error in %s: expected %s, got %s", aFunctionName, SSArgument::GetTypeName(aPreferred[i]), aRecieved[i].GetTypeName());
				success = false;
			}
		}

		return success;
	}
}