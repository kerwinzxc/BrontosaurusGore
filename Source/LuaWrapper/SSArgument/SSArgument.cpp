#include "stdafx.h"
#include "SSArgument.h"
#include "CommonUtilities/DL_Debug.h"

#ifdef new
#undef new
#endif // new

SSArgument::SSArgument(const SSArgument& anArgument) :SSArgument()
{
	myType = anArgument.GetType();
	switch (anArgument.GetType())
	{
	case eSSType::NUMBER:
		SetData(anArgument.GetData<ssLuaNumber>());
		break;
	case eSSType::STRING:
		SetData(anArgument.GetData<ssLuaString>());
		break;
	case eSSType::BOOL:
		SetData(anArgument.GetData<bool>());
		break;
	case eSSType::TABLE:
		SetData(anArgument.GetData<ssLuaTable>());
		break;
	case eSSType::NIL:
		break;
	default:
		DL_ASSERT("Unexpected default in switch in ssArgument in operator=(SSArgument anArgument) probably wrong type");
		break;
	}
}

SSArgument::SSArgument(SSArgument&& aTemporaryArgument)
{
	myData = aTemporaryArgument.myData;
	aTemporaryArgument.myData = nullptr;

	myType = aTemporaryArgument.myType;
	aTemporaryArgument.myType = eSSType::NIL;
}

SSArgument::SSArgument(const ssLuaNumber anArgument)
{
	myType = eSSType::NUMBER;
	myData = new double(anArgument);
}

SSArgument::SSArgument(const ssLuaString aString)
{
	myType = eSSType::STRING;
	myData = new ssLuaString(aString);
}

SSArgument::SSArgument(const bool aBool)
{
	myType = eSSType::BOOL;
	myData = new bool(aBool);
}

SSArgument::SSArgument(void* aLightUserData)
{
	myType = eSSType::LIGHTUSERDATA;
	myData = new void*(aLightUserData);
}

SSArgument::SSArgument(const ssLuaTable aTable)
{
	myType = eSSType::TABLE;
	myData = new ssLuaTable(aTable);
}

SSArgument& SSArgument::operator=(const SSArgument& anArgument)
{
	if (myType == anArgument.myType)
	{
		switch (myType)
		{
		case eSSType::NUMBER: 
			SetData(anArgument.GetData<ssLuaNumber>());
			break;
		case eSSType::STRING:
			SetData(anArgument.GetData<ssLuaString>());
			break;
		case eSSType::BOOL: 
			SetData(anArgument.GetData<bool>());
			break;
		case eSSType::LIGHTUSERDATA:
			SetData(anArgument.GetData<void*>());
			break;
		case eSSType::TABLE:
			SetData(anArgument.GetData<ssLuaTable>());
			break;
		case eSSType::NIL: 
			break;
		default: 
			DL_ASSERT(L"Unexpected default in switch in ssArgument in operator=(SSArgument anArgument) probably wrong type");
			break;
		}
	}
	else if (myType == eSSType::NIL)
	{
		switch (anArgument.GetType())
		{
		case eSSType::NUMBER: 
			SetData(anArgument.GetData<ssLuaNumber>());
			myType = eSSType::NUMBER;
			break;
		case eSSType::STRING: 
			SetData(anArgument.GetData<ssLuaString>());
			myType = eSSType::STRING;
			break;
		case eSSType::BOOL: 
			SetData(anArgument.GetData<bool>());
			myType = eSSType::BOOL;
			break;
		case eSSType::LIGHTUSERDATA:
			SetData(anArgument.GetData<void*>());
			myType = eSSType::LIGHTUSERDATA;
			break;
		case eSSType::TABLE:
			SetData(anArgument.GetData<ssLuaTable>());
			myType = eSSType::TABLE;
			break;
		case eSSType::NIL:
			break;
		default: 
			DL_ASSERT(L"Unexpected default in switch in ssArgument in operator=(SSArgument anArgument) probably wrong type");
			break;
		}
	}
	else
	{
		DL_ASSERT(L"Trying to assign a ssArgument to antoher with wrong type, plaease check that all of your types are correct");
	}

	return *this;
}

SSArgument & SSArgument::operator=(SSArgument && aTemporaryArgument)
{
	myData = aTemporaryArgument.myData;
	aTemporaryArgument.myData = nullptr;

	myType = aTemporaryArgument.myType;
	aTemporaryArgument.myType = eSSType::NIL;

	return *this;
}

SSArgument& SSArgument::operator=(const ssLuaNumber anArgument)
{
	if (myType == eSSType::NUMBER || myType == eSSType::NIL)
	{
		SetData(anArgument);
		myType = eSSType::NUMBER;
	}
	else
	{
		DL_ASSERT(L"Trying to assign something that isn't a number to an argument that is set to hold a number");
	}
	return *this;
}

SSArgument& SSArgument::operator=(const ssLuaString anArgument)
{
	if (myType == eSSType::STRING || myType == eSSType::NIL)
	{
		SetData(anArgument);
		myType = eSSType::STRING;
	}
	else
	{
		DL_ASSERT(L"Trying to assign something that isn't a string to an argument that is set to hold a string");
	}
	return *this;
}

SSArgument& SSArgument::operator=(const bool anArgument)
{
	if (myType == eSSType::BOOL || myType == eSSType::NIL)
	{
		SetData(anArgument);
		myType = eSSType::BOOL;
	}
	else
	{
		DL_ASSERT(L"Trying to assign something that isn't a bool to an argument that is set to hold a bool");
	}
	return *this;
}

SSArgument& SSArgument::operator=(void* aLightUserData)
{
	if (myType == eSSType::LIGHTUSERDATA || myType == eSSType::NIL)
	{
		SetData(aLightUserData);
		myType = eSSType::LIGHTUSERDATA;
	}
	else
	{
		DL_ASSERT(L"Trying to assign something that isn't light user data to an argument that is set to hold light user data");
	}

	return *this;
}

SSArgument& SSArgument::operator=(const ssLuaTable& aTable)
{
	if (myType == eSSType::TABLE || myType == eSSType::NIL)
	{
		SetData(aTable);
		myType = eSSType::TABLE;
	}
	else
	{
		DL_ASSERT(L"Trying to assign something that isn't a table to an argument that is set to hold a table");
	}
	return *this;
}

const short SSArgument::GetShort() const
{
	if (myType != eSSType::NUMBER)
	{
		DL_ASSERT("trying to get a number from an argument that isn't a number");
	}
	return static_cast<short>(GetData<ssLuaNumber>());
}

const int SSArgument::GetInt() const
{
	if (myType != eSSType::NUMBER)
	{
		DL_ASSERT("trying to get a number from an argument that isn't a number");
	}
	return static_cast<int>(GetData<ssLuaNumber>());
}

const unsigned int SSArgument::GetUInt() const
{
	if (myType != eSSType::NUMBER)
	{
		DL_ASSERT("trying to get a number from an argument that isn't a number");
	}
	return static_cast<unsigned int>(GetData<ssLuaNumber>());
}

const double SSArgument::GetDouble() const
{
	if (myType != eSSType::NUMBER)
	{
		DL_ASSERT("trying to get a number from an argument that isn't a number");
	}
	return GetData<ssLuaNumber>();
}

const float SSArgument::GetFloat() const
{
	if (myType != eSSType::NUMBER)
	{
		DL_ASSERT("trying to get a number from an argument that isn't a number");
	}
	return static_cast<float>(GetData<ssLuaNumber>());
}

const unsigned short SSArgument::GetUShort() const
{
	if (myType != eSSType::NUMBER)
	{
		DL_ASSERT("trying to get a number from an argument that isn't a number");
	}
	return static_cast<unsigned short>(GetData<ssLuaNumber>());
}

ssLuaNumber SSArgument::GetNumber() const
{
	if (myType != eSSType::NUMBER)
	{
		DL_ASSERT("trying to get a number from an argument that isn't a number");
	}
	return GetData<ssLuaNumber>();
}

ssLuaString SSArgument::GetString() const
{
	if (myType != eSSType::STRING)
	{
		DL_ASSERT("trying to get wrong type from argument");
	}
	return GetData<ssLuaString>();
}

const ssLuaTable& SSArgument::GetTable() const
{
	if (myType != eSSType::TABLE)
	{
		DL_ASSERT("trying to get wrong type from argument");
	}
	return GetData<ssLuaTable>();
}

const bool SSArgument::GetBool() const
{
	if (myType != eSSType::BOOL)
	{
		DL_ASSERT("trying to get wrong type from argument");
	}
	return GetData<bool>();
}

void* SSArgument::GetUserData() const
{
	if (myType != eSSType::LIGHTUSERDATA)
	{
		DL_ASSERT("trying to get light user data from %s", GetTypeName());
		return nullptr;
	}

	return *static_cast<void**>(myData);
}

void SSArgument::SetToNil()
{
	if (myData != nullptr)
	{
		delete myData;
		myData = nullptr;
		myType = eSSType::NIL;
	}
}

const char* SSArgument::GetTypeName() const
{
	return GetTypeName(myType);
}

const char* SSArgument::GetTypeName(const eSSType aType)
{
	static const char* nilStr = "nil";
	static const char* numberStr = "number";
	static const char* stringStr = "string";
	static const char* boolStr = "bool";
	static const char* tableStr = "table";
	static const char* undefinedStr = "undefined";

	switch (aType) //TODO: this is unsafe, returning a pointer to a local variable
	{
	case eSSType::NIL: return nilStr;
	case eSSType::NUMBER: return numberStr;
	case eSSType::STRING: return stringStr;
	case eSSType::BOOL: return boolStr;
	case eSSType::TABLE: return tableStr;
	default: return undefinedStr;
	}
}

SSArgument::~SSArgument()
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
const TYPE& SSArgument::GetData() const
{
	if (myData == nullptr || myType == eSSType::NIL)
	{
		DL_ASSERT("Got nil when expected som kind of data in Get Data on a lua argument");
	}
	TYPE* temp = static_cast<TYPE*>(myData);
	return *temp;
}

template <typename TYPE>
void SSArgument::SetData(const TYPE& someData)
{
	if (myData == nullptr)
	{
		myData = new TYPE;
	}
	TYPE* tempPtr = static_cast<TYPE*>(myData);
	*tempPtr = someData;
}
