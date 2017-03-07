#include "stdafx.h"
#include "ConsolePrinter.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../PostMaster/MessageType.h"
#include <iostream>
#include "../ThreadedPostmaster/PrintMessage.h"

CommonUtilities::CConsolePrinter* CommonUtilities::CConsolePrinter::ourInstance = nullptr;

void CommonUtilities::CConsolePrinter::Create()
{
	ourInstance = new CConsolePrinter;
}

void CommonUtilities::CConsolePrinter::Destroy()
{
	SAFE_DELETE(ourInstance);
}

CommonUtilities::CConsolePrinter& CommonUtilities::CConsolePrinter::GetInstance()
{
	if(ourInstance == nullptr)
	{
		Create();
	}
	return *ourInstance;
}

eMessageReturn CommonUtilities::CConsolePrinter::DoEvent(const Postmaster::Message::CPrintMessage& aPrintMessage)
{
	std::cout << aPrintMessage.GetText() << std::endl;
	return eMessageReturn::eContinue;
}

CommonUtilities::CConsolePrinter::CConsolePrinter()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::ePrint);
}


CommonUtilities::CConsolePrinter::~CConsolePrinter()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}
