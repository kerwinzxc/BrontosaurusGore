#include "PrintConsole.h"
#include <iostream>
#include "PrintMessage.h"
#include "Postmaster.h"


Postmaster::CPrintConsole::CPrintConsole() : ISubscriber()
{
}

Postmaster::CPrintConsole::~CPrintConsole()
{
}

void Postmaster::CPrintConsole::DoMessage(const Message::CPrintMessage& aPrintMessage)
{
	std::cout << aPrintMessage.GetText() << std::endl;
}
