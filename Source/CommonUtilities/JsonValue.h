#pragma once

namespace picojson
{
	class value;
}

namespace CU
{
	template<typename T>
	class Vector2;
	using Vector2f = Vector2<float>;
	template<typename T>
	class Vector3;
	using Vector3f = Vector3<float>;
	template<typename T>
	class Vector4;
	using Vector4f = Vector4<float>;

	enum class eJsoneValueType
	{
		JSON_NULL,
		BOOL,
		NUMBER,
		STRING,
		ARRAY,
		OBJECT
	};

	class CJsonValue
	{
	public:
		CJsonValue();
		CJsonValue(const CJsonValue& aValue);
		CJsonValue(const std::string& aFilePath);
		~CJsonValue();

		CJsonValue& operator=(const CJsonValue& aValue);

		std::string Parse(const std::string& aFilePath);

		eJsoneValueType GetType() const;
		int Size() const;

		int Count(const std::string& aKey) const;

		bool IsNull() const;
		bool IsBool() const;
		bool IsNumber() const;
		bool IsString() const;
		bool IsArray() const;
		bool IsObject() const;
		bool HasKey(const std::string& aKey) const;
		bool HasIndex(const int aIndex) const;

		bool GetBool() const;

		double GetNumber() const;
		float GetFloat() const;
		int GetInt() const;
		unsigned int GetUInt() const;
		const unsigned char GetUchar() const;
		short GetShort() const;

		template<typename T>
		T GetEnum();

		const std::string& GetString() const;
		const std::string& TryGetString() const;

		Vector2f GetVector2f(const std::string& aNameXY = "xy") const;
		Vector3f GetVector3f(const std::string& aNameXYZ = "xyz") const;
		Vector4f GetVector4f(const std::string& aNameXYZW = "xyzw") const;

		CJsonValue operator[](const int aIndex) const;
		CJsonValue operator[](const std::string& aKey) const;

		CJsonValue at(const int aIndex) const;
		CJsonValue at(const std::string& aKey) const;
		bool CheckKeys(CU::GrowingArray<std::string> someKeys) const;
	private:
		CJsonValue(const picojson::value* aValuePointer);

		const picojson::value* myValue;
		bool myIsBorrowed;
	};

	template<typename T>
	inline T CJsonValue::GetEnum()
	{
		return static_cast<T>(GetInt());
	}
}
