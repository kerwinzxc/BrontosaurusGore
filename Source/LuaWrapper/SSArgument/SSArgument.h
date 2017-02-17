// 
// 2016-08-28
#pragma once
#include <unordered_map>

struct SSArgument;

typedef std::unordered_map<std::string, SSArgument> ssLuaTable;
typedef double ssLuaNumber;
typedef const char* ssLuaString;


enum class eSSType
{
	NIL,
	NUMBER,
	STRING,
	BOOL,
	TABLE,
	LIGHTUSERDATA
};

struct SSArgument
{
	SSArgument():myData(nullptr), myType(eSSType::NIL)
	{
	}

	SSArgument(const SSArgument& anArgument);
	SSArgument(SSArgument&& aTemporaryArgument);
	SSArgument(const ssLuaNumber aNumber);
	SSArgument(const ssLuaString aString);
	SSArgument(const bool aBool);
	SSArgument(void* aLightUserData);
	SSArgument(const ssLuaTable aTable);

	~SSArgument();

	SSArgument& operator=(const SSArgument& anArgument);
	SSArgument& operator=(SSArgument&& aTemporaryArgument);
	SSArgument& operator=(const ssLuaNumber aNumber);
	SSArgument& operator=(const ssLuaString aString);
	SSArgument& operator=(const bool aBool);
	SSArgument& operator=(void* aLightUserData);
	SSArgument& operator=(const ssLuaTable& aTable);

	const short GetShort() const;
	const unsigned short SSArgument::GetUShort() const;
	const int GetInt() const;
	const unsigned int GetUInt() const;
	const float GetFloat() const;
	const double GetDouble() const;
	ssLuaNumber GetNumber() const;
	ssLuaString GetString() const;
	const ssLuaTable& GetTable() const;
	const bool GetBool() const;
	void* GetUserData() const;

	std::string AsString() const;
	
	void SetToNil();

	eSSType GetType() const
	{
		return myType;
	}

	const char* GetTypeName() const;
	static const char* GetTypeName(const eSSType aType);
private:

	template<typename TYPE>
	const TYPE& GetData()const;

	template<typename TYPE>
	void SetData(const TYPE& someData);

	void* myData;
	eSSType myType;
};

//------------ moved these to cpp file since they are private

//template <typename TYPE>
///*
//* WARNING: will note check type
//* Make sure you have checked that the argument is of the correct type with GetType!!
//* Asserts if data is a nullptr or type is nil
//*/
//TYPE SSArgument::GetData() const
//{
//	if (myData == nullptr || myType == eSSType::NIL)
//	{
//		DL_ASSERT("Got nil when expected som kind of data in Get Data on a lua argument");
//	}
//	TYPE* temp = static_cast<TYPE*>(myData);
//	return *temp;
//}
//
//template <typename TYPE>
//void SSArgument::SetData(TYPE someData)
//{
//	if (myData == nullptr)
//	{
//		myData = new TYPE;
//	}
//	TYPE* tempPtr = static_cast<TYPE*>(myData);
//	*tempPtr = someData;
//}
