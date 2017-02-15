#pragma once

enum class eComponentMessageType;

namespace ComponentMessage
{
	int GetType(const std::string& aTypeName);
	bool GetTypeName(const eComponentMessageType aType, std::string& aNameOut);
	void PrintComponentsToFile(const std::string& aFilePath);
}