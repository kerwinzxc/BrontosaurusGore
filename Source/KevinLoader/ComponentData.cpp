#include "stdafx.h"
#include "ComponentData.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/DynamicString.h"


namespace KLoader
{


	CComponentData::CComponentData(const CComponentData& anArgument) :CComponentData()
	{
		myType = anArgument.GetType();
		switch (anArgument.GetType())
		{
		case eCompDataType::NUMBER:
			SetData(anArgument.GetData<CompNumber>());
			break;
		case eCompDataType::STRING:
			SetData(anArgument.GetData<CompString>());
			break;
		case eCompDataType::BOOL:
			SetData(anArgument.GetData<bool>());
			break;
		case eCompDataType::NIL:
			break;
		default:
			DL_ASSERT("Unexpected default in switch in CComponentData in operator=(CComponentData anArgument) probably wrong type");
			break;
		}
	}

	CComponentData::CComponentData(CComponentData&& aTemporaryArgument)
	{
		myData = aTemporaryArgument.myData;
		aTemporaryArgument.myData = nullptr;

		myType = aTemporaryArgument.myType;
		aTemporaryArgument.myType = eCompDataType::NIL;
	}

	CComponentData::CComponentData(const CompNumber anArgument)
	{
		myType = eCompDataType::NUMBER;
		myData = new double(anArgument);
	}

	CComponentData::CComponentData(const CompString aString)
	{
		myType = eCompDataType::STRING;
		myData = new CompString(aString);
	}

	CComponentData::CComponentData(const bool aBool)
	{
		myType = eCompDataType::BOOL;
		myData = new bool(aBool);
	}

	CComponentData::CComponentData(const CompArray anArray)
	{
		myType = eCompDataType::ARRAY;
		myData = new CompArray(anArray);
	}

	CComponentData& CComponentData::operator=(const CComponentData& anArgument)
	{
		if (myType == anArgument.myType)
		{
			switch (myType)
			{
			case eCompDataType::NUMBER:
				SetData(anArgument.GetData<CompNumber>());
				break;
			case eCompDataType::STRING:
				SetData(anArgument.GetData<CompString>());
				break;
			case eCompDataType::BOOL:
				SetData(anArgument.GetData<bool>());
				break;
			case eCompDataType::NIL:
				break;
			default:
				DL_ASSERT(L"Unexpected default in switch in CComponentData in operator=(CComponentData anArgument) probably wrong type");
				break;
			}
		}
		else if (myType == eCompDataType::NIL)
		{
			switch (anArgument.GetType())
			{
			case eCompDataType::NUMBER:
				SetData(anArgument.GetData<CompNumber>());
				myType = eCompDataType::NUMBER;
				break;
			case eCompDataType::STRING:
				SetData(anArgument.GetData<CompString>());
				myType = eCompDataType::STRING;
				break;
			case eCompDataType::BOOL:
				SetData(anArgument.GetData<bool>());
				myType = eCompDataType::BOOL;
				break;
			case eCompDataType::NIL:
				break;
			default:
				DL_ASSERT(L"Unexpected default in switch in CComponentData in operator=(CComponentData anArgument) probably wrong type");
				break;
			}
		}
		else
		{
			DL_ASSERT(L"Trying to assign a CComponentData to antoher with wrong type, plaease check that all of your types are correct");
		}

		return *this;
	}

	CComponentData & CComponentData::operator=(CComponentData && aTemporaryArgument)
	{
		myData = aTemporaryArgument.myData;
		aTemporaryArgument.myData = nullptr;

		myType = aTemporaryArgument.myType;
		aTemporaryArgument.myType = eCompDataType::NIL;

		return *this;
	}

	CComponentData& CComponentData::operator=(const CompNumber anArgument)
	{
		if (myType == eCompDataType::NUMBER || myType == eCompDataType::NIL)
		{
			SetData(anArgument);
			myType = eCompDataType::NUMBER;
		}
		else
		{
			DL_ASSERT(L"Trying to assign something that isn't a number to an argument that is set to hold a number");
		}
		return *this;
	}

	CComponentData& CComponentData::operator=(const CompString anArgument)
	{
		if (myType == eCompDataType::STRING || myType == eCompDataType::NIL)
		{
			SetData(anArgument);
			myType = eCompDataType::STRING;
		}
		else
		{
			DL_ASSERT(L"Trying to assign something that isn't a string to an argument that is set to hold a string");
		}
		return *this;
	}

	CComponentData& CComponentData::operator=(const bool anArgument)
	{
		if (myType == eCompDataType::BOOL || myType == eCompDataType::NIL)
		{
			SetData(anArgument);
			myType = eCompDataType::BOOL;
		}
		else
		{
			DL_ASSERT(L"Trying to assign something that isn't a bool to an argument that is set to hold a bool");
		}
		return *this;
	}

	CComponentData& CComponentData::operator=(const CompArray anArray)
	{
		if (myType == eCompDataType::ARRAY || myType == eCompDataType::NIL)
		{
			SetData(anArray);
			myType = eCompDataType::ARRAY;
		}
		else
		{
			DL_ASSERT(L"Trying to assign something that isn't an arra to an argument that is set to hold an array");
		}
		return *this;
	}

	const short CComponentData::GetShort() const
	{
		if (myType != eCompDataType::NUMBER)
		{
			DL_ASSERT("trying to get a number from an argument that isn't a number");
		}
		return static_cast<short>(GetData<CompNumber>());
	}

	const int CComponentData::GetInt() const
	{
		if (myType != eCompDataType::NUMBER)
		{
			DL_ASSERT("trying to get a number from an argument that isn't a number");
		}
		return static_cast<int>(GetData<CompNumber>());
	}

	const unsigned int CComponentData::GetUInt() const
	{
		if (myType != eCompDataType::NUMBER)
		{
			DL_ASSERT("trying to get a number from an argument that isn't a number");
		}
		return static_cast<unsigned int>(GetData<CompNumber>());
	}

	const double CComponentData::GetDouble() const
	{
		if (myType != eCompDataType::NUMBER)
		{
			DL_ASSERT("trying to get a number from an argument that isn't a number");
		}
		return GetData<CompNumber>();
	}

	const float CComponentData::GetFloat() const
	{
		if (myType != eCompDataType::NUMBER)
		{
			DL_ASSERT("trying to get a number from an argument that isn't a number");
		}
		return static_cast<float>(GetData<CompNumber>());
	}

	const unsigned short CComponentData::GetUShort() const
	{
		if (myType != eCompDataType::NUMBER)
		{
			DL_ASSERT("trying to get a number from an argument that isn't a number");
		}
		return static_cast<unsigned short>(GetData<CompNumber>());
	}

	CompNumber CComponentData::GetNumber() const
	{
		if (myType != eCompDataType::NUMBER)
		{
			DL_ASSERT("trying to get a number from an argument that isn't a number");
		}
		return GetData<CompNumber>();
	}

	CompString CComponentData::GetString() const
	{
		if (myType != eCompDataType::STRING)
		{
			DL_ASSERT("trying to get wrong type from argument");
		}
		return GetData<CompString>();
	}

	const bool CComponentData::GetBool() const
	{
		if (myType != eCompDataType::BOOL)
		{
			DL_ASSERT("trying to get wrong type from argument");
		}
		return GetData<bool>();
	}

	CompArray CComponentData::GetArray() const
	{
		if (myType != eCompDataType::ARRAY)
		{
			DL_ASSERT("trying to get wrong type from argument");
		}
		return GetData<CompArray>();
	}

	void CComponentData::SetToNil()
	{
		if (myData != nullptr)
		{
			delete myData;
			myData = nullptr;
			myType = eCompDataType::NIL;
		}
	}

	const char* CComponentData::GetTypeName() const
	{
		return GetTypeName(myType);
	}

	const char* CComponentData::GetTypeName(const eCompDataType aType)
	{
		switch (aType) //TODO: this is unsafe, returning a pointer to a local variable
		{
		case eCompDataType::NIL: return "nil";
		case eCompDataType::NUMBER: return "number";
		case eCompDataType::STRING: return "string";
		case eCompDataType::BOOL: return "bool";
		case eCompDataType::ARRAY: return "array";
		default: return "undefined";
		}
	}

	CComponentData::~CComponentData()
	{
		if (myData != nullptr)
		{
			delete myData;
			myData = nullptr;
		}
	}

	template <typename TYPE>
	/*
	* WARNING: will note check type
	* Make sure you have checked that the argument is of the correct type with GetType!!
	* Asserts if data is a nullptr or type is nil
	*/
	const TYPE& CComponentData::GetData() const
	{
		if (myData == nullptr || myType == eCompDataType::NIL)
		{
			DL_ASSERT("Got nil when expected som kind of data in Get Data on a lua argument");
		}
		TYPE* temp = static_cast<TYPE*>(myData);
		return *temp;
	}

	template <typename TYPE>
	void CComponentData::SetData(const TYPE& someData)
	{
		if (myData == nullptr)
		{
			myData = new TYPE;
		}
		TYPE* tempPtr = static_cast<TYPE*>(myData);
		*tempPtr = someData;
	}

	
}
