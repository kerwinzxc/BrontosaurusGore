#include "stdafx.h"
#include "Chat.h"
#include <iostream>
#include <thread>


CChat::CChat(): myThread(nullptr)
{
	myIsRunnning = false;
}


CChat::~CChat()
{
	if (myThread != nullptr)
	{
		myIsRunnning = false;
		myThread->join();
		delete myThread;
	}
}

void CChat::StartChat()
{
	if (myThread == nullptr)
	{
		myIsRunnning = true;
		myThread = new std::thread(&CChat::Update, this);
	}
}

void CChat::StopChat()
{
	if (myThread != nullptr)
	{
		myIsRunnning = false;
		myThread->join();
		delete myThread;
		myThread = nullptr;
	}
}

void CChat::Update()
{
	while (myIsRunnning == true)
	{
		std::getline(std::cin, myCurrentMessage) ;

		myWaiting = true;
		while (myWaiting == true)
		{
			std::this_thread::yield();
		}
	}
}

std::string CChat::GetChatMessage()
{
	if (myWaiting == true)
	{
		myWaiting = false;
		return myCurrentMessage;
	}

	return "";
}
