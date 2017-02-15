#pragma once
#include "../CommonUtilities/GrowingArray.h"

namespace KLoader
{
	class CComponentData;
	typedef CU::GrowingArray<CComponentData> CompArray;
	typedef double CompNumber;
	typedef const char* CompString;

	enum class eCompDataType
	{
		NIL,
		NUMBER,
		STRING,
		BOOL,
		ARRAY
	};

	class CComponentData
	{
	public:
		CComponentData() :myData(nullptr), myType(eCompDataType::NIL)
		{
		}

		CComponentData(const CComponentData& anArgument);
		CComponentData(CComponentData&& aTemporaryArgument);
		CComponentData(const CompNumber aNumber);
		CComponentData(const CompString aString);
		CComponentData(const bool aBool);
		CComponentData(const CompArray anArray);

		~CComponentData();

		CComponentData& operator=(const CComponentData& anArgument);
		CComponentData& operator=(CComponentData&& aTemporaryArgument);
		CComponentData& operator=(const CompNumber aNumber);
		CComponentData& operator=(const CompString aString);
		CComponentData& operator=(const bool aBool);
		CComponentData& operator=(const CompArray anArray);

		const short GetShort() const;
		const unsigned short CComponentData::GetUShort() const;
		const int GetInt() const;
		const unsigned int GetUInt() const;
		const float GetFloat() const;
		const double GetDouble() const;
		CompNumber GetNumber() const;
		CompString GetString() const;
		const bool GetBool() const;
		CompArray GetArray()const;

		void SetToNil();

		eCompDataType GetType() const
		{
			return myType;
		}

		const char* GetTypeName() const;
		static const char* GetTypeName(const eCompDataType aType);
	private:

		template<typename TYPE>
		const TYPE& GetData()const;

		template<typename TYPE>
		void SetData(const TYPE& someData);

		void* myData;
		eCompDataType myType;
	};
}
