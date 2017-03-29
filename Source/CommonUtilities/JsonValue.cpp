#include "stdafx.h"
#include "JsonValue.h"
#include "picojson.h"
#include <fstream>

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

//#define JSON_ERROR(ERROR_MESSAGE) assert(ERROR_MESSAGE && false)
#define JSON_ERROR DL_MESSAGE_BOX
#define CHAR_TO_STR(A_CHAR) std::string(1, A_CHAR)

namespace CU
{
	CJsonValue::CJsonValue()
		: myValue(nullptr)
		, myIsBorrowed(false)
	{
		//myValue = new picojson::value();
	}

	CJsonValue::CJsonValue(const CJsonValue& aValue)
		: myValue(aValue.myValue)
		, myIsBorrowed(true)
	{
		//if (aValue.myValue != nullptr)
		//{
		//	myValue = new picojson::value(*aValue.myValue);
		//}
	}

	CJsonValue::CJsonValue(const std::string& aFilePath)
		: myValue(nullptr)
		, myIsBorrowed(false)
	{
		std::string errroMessage = Parse(aFilePath);
		if (!errroMessage.empty())
		{
			DL_MESSAGE_BOX("Error parsing json file %s: %s", aFilePath.c_str(), errroMessage.c_str());
		}
	}

	CJsonValue::~CJsonValue()
	{
		if (myIsBorrowed == false)
		{
			SAFE_DELETE(myValue);
		}
	}

	CJsonValue& CJsonValue::operator=(const CJsonValue& aValue)
	{
		myValue = aValue.myValue;
		myIsBorrowed = true;

		return *this;
	}

	std::string CJsonValue::Parse(const std::string& aFilePath)
	{
		std::ifstream jsonFile(aFilePath);
		if (jsonFile.good() == false)
		{
			JSON_ERROR("Could not find json file %s", aFilePath.c_str());
			return "Error loading filePath " + aFilePath;
		}

		if (myValue != nullptr && myIsBorrowed == false)
		{
			delete myValue;
		}

		myIsBorrowed = false;

		picojson::value* newValue = new picojson::value();
		std::string errorMessage = picojson::parse(*newValue, jsonFile);
		jsonFile.close();
		myValue = newValue;

		return errorMessage;
	}

	eJsoneValueType CJsonValue::GetType() const
	{
		if (IsObject() == true)
		{
			return eJsoneValueType::OBJECT;
		}

		if (IsArray() == true)
		{
			return eJsoneValueType::ARRAY;
		}

		if (IsString() == true)
		{
			return eJsoneValueType::STRING;
		}

		if (IsNumber() == true)
		{
			return eJsoneValueType::NUMBER;
		}

		if (IsBool() == true)
		{
			return eJsoneValueType::BOOL;
		}

		return eJsoneValueType::JSON_NULL;
	}

	int CJsonValue::Size() const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return 0;
		}

		if (IsArray() == true)
		{
			return static_cast<int>(myValue->get<picojson::array>().size());
		}

		if (IsObject() == true)
		{
			return static_cast<int>(myValue->get<picojson::object>().size());
		}

		if (IsString() == true)
		{
			return static_cast<int>(myValue->get<std::string>().size());
		}

		JSON_ERROR("trying to get size from simple json value");
		return 0;
	}

	int CJsonValue::Count(const std::string& aKey) const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return 0;
		}
		if (IsObject() == false)
		{
			JSON_ERROR("json value is not an object, it's %s", myValue->to_str().c_str());
			return 0;
		}

		const picojson::object& tempObject = myValue->get<picojson::object>();
		return static_cast<int>(tempObject.count(aKey));
	}

	bool CJsonValue::IsNull() const
	{
		return myValue == nullptr || myValue->is<picojson::null>();
	}

	bool CJsonValue::IsBool() const
	{
		return myValue != nullptr && myValue->is<bool>();
	}
	bool CJsonValue::IsNumber() const
	{
		return myValue != nullptr && myValue->is<double>();
	}

	bool CJsonValue::IsString() const
	{
		return myValue != nullptr && myValue->is<std::string>();
	}

	bool CJsonValue::IsArray() const
	{
		return myValue != nullptr && myValue->is<picojson::array>();
	}

	bool CJsonValue::IsObject() const
	{
		return myValue != nullptr && myValue->is<picojson::object>();
	}

	bool CJsonValue::HasKey(const std::string& aKey) const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return false;
		}
		if (IsObject() == false)
		{
			JSON_ERROR("json value is not an object, it's %s", myValue->to_str().c_str());
			return false;
		}

		const picojson::object& tempObject = myValue->get<picojson::object>();
		auto it = tempObject.find(aKey);

		return it != tempObject.end();
	}

	bool CJsonValue::HasIndex(const int aIndex) const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return false;
		}
		if (IsArray() == false)
		{
			JSON_ERROR("json value is not an array, it's %s", myValue->to_str().c_str());
			return false;
		}

		return aIndex >= 0 && aIndex < Size();
	}

	bool CJsonValue::GetBool() const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return false;
		}
		if (IsBool() == false)
		{
			JSON_ERROR("json value is not bool, it's %s", myValue->to_str().c_str());
			return false;
		}

		return myValue->get<bool>();
	}

	double CJsonValue::GetNumber() const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return 0.0;
		}
		if (IsNumber() == false)
		{
			JSON_ERROR("json value is not number, it's %s", myValue->to_str().c_str());
			return 0.0;
		}

		return myValue->get<double>();
	}

	float CJsonValue::GetFloat() const
	{
		return static_cast<float>(GetNumber());
	}

	int CJsonValue::GetInt() const
	{
		return static_cast<int>(GetNumber());
	}

	unsigned int CJsonValue::GetUInt() const
	{
		return static_cast<unsigned int>(GetNumber());
	}

	const std::string& CJsonValue::GetString() const
	{
		const static std::string nullString("");

		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return nullString;
		}
		if (IsString() == false)
		{
			JSON_ERROR("json value is not string, returning \"\", it's %s", myValue->to_str().c_str());
			return nullString;
		}

		return myValue->get<std::string>();
	}

	const std::string& CJsonValue::TryGetString() const
	{
		if (IsString() == false)
		{
			const static std::string nullString("");
			return nullString;
		}

		return myValue->get<std::string>();
	}
	
	Vector2f CJsonValue::GetVector2f(const std::string& aNameXY) const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return Vector2f();
		}
		if (IsObject() == false)
		{
			eJsoneValueType type = GetType();
			JSON_ERROR("json value is not an object, it's %s", myValue->to_str().c_str());
			type = eJsoneValueType::JSON_NULL; //remove warning
			return Vector2f();
		}
		if (Size() != 2)
		{
			JSON_ERROR("json value does not have 2 elements");
			return Vector2f();
		}
		assert(aNameXY.size() == 2u && "vector2 name does not match vector 2");

		return Vector2f(at(aNameXY.substr(0, 1)).GetFloat(), at(aNameXY.substr(1, 1)).GetFloat());
	}
	
	Vector3f CJsonValue::GetVector3f(const std::string& aNameXYZ) const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return Vector3f();
		}
		if (IsObject() == false)
		{
			eJsoneValueType type = GetType();
			JSON_ERROR("json value is not an array, it's %s", myValue->to_str().c_str());
			type = eJsoneValueType::JSON_NULL; //remove warning
			return Vector3f();
		}
		if (Size() != 3)
		{
			JSON_ERROR("json value does not have three elements");
			return Vector3f();
		}
		assert(aNameXYZ.size() == 3u && "vector3 name does not match vector3");
		return Vector3f(at(aNameXYZ.substr(0, 1)).GetFloat(), at(aNameXYZ.substr(1, 1)).GetFloat(), at(aNameXYZ.substr(2, 1)).GetFloat());
	}

	Vector4f CJsonValue::GetVector4f(const std::string& aNameXYZW) const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return Vector4f();
		}
		if (IsObject() == false)
		{
			JSON_ERROR("json value is not an object, it's %s", myValue->to_str().c_str());
			return Vector4f();
		}
		if (Size() != 4)
		{
			JSON_ERROR("json value does not have 4 elements");
			return Vector4f();
		}
		assert(aNameXYZW.size() == 4u && "vector4 name does not match vector 4");

		return Vector4f(at(aNameXYZW.substr(0, 1)).GetFloat(), at(aNameXYZW.substr(1, 1)).GetFloat(), at(aNameXYZW.substr(2, 1)).GetFloat(), at(aNameXYZW.substr(3, 1)).GetFloat());
	}

	CJsonValue CJsonValue::operator[](const int anIndex) const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return CJsonValue();
		}
		if (IsArray() == false)
		{
			JSON_ERROR("json value is not an array, it's %s", myValue->to_str().c_str());
			return CJsonValue();
		}

		size_t sIndex = static_cast<size_t>(anIndex);

		const picojson::array& tempArray = myValue->get<picojson::array>();
		if (sIndex < 0 || sIndex >= tempArray.size())
		{
			JSON_ERROR("json array subscript out of range");
			return CJsonValue();
		}

		return CJsonValue(&tempArray[sIndex]);
	}

	CJsonValue CJsonValue::operator[](const std::string& aKey) const
	{
		if (IsNull() == true)
		{
			JSON_ERROR("json value is null");
			return CJsonValue();
		}
		if (IsObject() == false)
		{
			JSON_ERROR("json value is not an object, it's %s", myValue->to_str().c_str());
			return CJsonValue();
		}

		const picojson::object& tempObject = myValue->get<picojson::object>();
		auto it = tempObject.find(aKey);
		if (it == tempObject.end())
		{
			JSON_ERROR("json object invalid key");
			return CJsonValue();
		}

		return CJsonValue(&it->second);
	}

#define self (*this)
	CJsonValue CJsonValue::at(const int aIndex) const
	{
		return self[aIndex];
	}

	CJsonValue CJsonValue::at(const std::string& aKey) const
	{
		return self[aKey];
	}

	bool CJsonValue::CheckKeys(CU::GrowingArray<std::string> someKeys) const
	{
#ifdef _DEBUG 
		for (CU::GrowingArray<std::string>::size_type i = 0; i < someKeys.Size(); ++i)
		{
			if (Count(someKeys[i]) == 0)
			{
				return false;
			}
		}
#endif //_DEBUG

		return true;
	}
#undef self

	CJsonValue::CJsonValue(const picojson::value* aValuePointer)
		: myValue(aValuePointer)
		, myIsBorrowed(true)
	{
	}
}