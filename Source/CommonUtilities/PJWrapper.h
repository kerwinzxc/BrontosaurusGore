#pragma once
#include "picojson.h"

namespace CU
{
	class CPJWrapper;
	typedef CPJWrapper JsonObject;

	enum class eJsonType
	{
		JSON_NULL,
		BOOL,
		NUMBER,
		STRING,
		ARRAY,
		OBJECT,

	};

	class CPJWrapper
	{
	public:
		CPJWrapper();
		CPJWrapper(const picojson::value& aValue);
		~CPJWrapper();

		bool Parse(std::string afilePath, std::string* anErrorString = nullptr);

		eJsonType GetType();
		CU::eJsonType GetType() const;
		bool IsNull() const;

		bool IsBool() const;
		bool GetBool();

		bool IsNumber() const;
		double GetNumber();
		double GetNumber() const;
		float GetFloat() const;
		int GetInt() const;
		unsigned int GetUInt() const;

		bool IsString() const;
		std::string& GetString();
		const std::string& GetString() const;
		bool IsArray() const;
		CPJWrapper operator[](const int anIndex);
		CPJWrapper operator[](const int anIndex) const;
		CPJWrapper operator[](std::string aKey);
		CPJWrapper operator[](std::string aKey) const;
		size_t Size();
		size_t Size() const;
		bool IsObject() const;
		JsonObject GetJsonObject();
		JsonObject GetJsonObject()const;

		CPJWrapper at(std::string aKey);
		CPJWrapper at(std::string aKey)const;

		unsigned count(std::string aKey);
		unsigned count(std::string aKey)const;

		
	private:
		picojson::value myValue;
	};

}
