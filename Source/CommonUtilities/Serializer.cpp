#include "stdafx.h"
#include "Serializer.h"

namespace CU
{
	GrowingArray<char> ISerializer::ReadFile(const std::string& aFilePath)
	{
		GrowingArray<char> buffer;

		std::ifstream file(aFilePath, std::ios::binary);
		if (!file.good())
		{
			DL_MESSAGE_BOX("Failed to open file for reading: %s", aFilePath.c_str());
			return buffer;
		}

		file.seekg(0, std::ios::end);
		long long tellgResult = file.tellg();
		file.seekg(0, std::ios::beg);

		tellgResult -= file.tellg();

		if (tellgResult <= 0)
		{
			return buffer;
		}

		unsigned int bufferSize = static_cast<unsigned int>(tellgResult);
		buffer.Init(bufferSize);
		buffer.Resize(bufferSize);

		file.read(buffer.AsPointer(), tellgResult);
		if (!file.good())
		{
			DL_MESSAGE_BOX("Failed to read from file: %s", aFilePath.c_str());
			buffer.Destroy();
			return buffer;
		}

		return buffer;
	}

	bool ISerializer::WriteFile(const CU::GrowingArray<char>& aBuffer, const std::string& aFilePath)
	{
		std::ofstream file(aFilePath, std::ios::binary);
		if (!file.good())
		{
			DL_MESSAGE_BOX("Failed to open file for writing: %s", aFilePath.c_str());
			return false;
		}

		file.write(aBuffer.AsPointer(), aBuffer.ByteSize());
		if (!file.good())
		{
			DL_MESSAGE_BOX("Failed to write to file: %s", aFilePath.c_str());
		}

		return true;
	}
}