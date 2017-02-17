#include "stdafx.h"
#include "ServerMain.h"

int main()
{
	CServerMain serverMain;
	serverMain.StartServer();

	serverMain.Update();

	system("pause");
	return 0;
}
