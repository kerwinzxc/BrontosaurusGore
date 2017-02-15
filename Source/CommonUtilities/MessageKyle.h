#pragma once
#include "CommonUtilities.h"

class Message
{
public:
	Message(){ myData = nullptr; }
	~Message(){}

	Message(eMsg aMsgType, void* aData, bool aShouldDelete)
	{
		myMsgType = aMsgType;
		myData = aData;
		myShouldDelete = aShouldDelete;
	}

	inline eMsg GetMsgType() const
	{
		return myMsgType;
	}

	inline void* GetData() const
	{
		return myData;
	}

	void DeleteData()
	{
		if (myShouldDelete == true)
		{
			SAFE_DELETE(myData);
		}
	}

private:
	bool myShouldDelete;
	eMsg myMsgType;
	void* myData;
};