#include "stdafx.h"
#include "Client.h"
#include <cstdlib>
#include <iostream>
#include <string>

int main()
{
	CClient client;
	client.StartClient();

	std::string ip;

	std::cout << "Serve IP: ";
	getline(std::cin, ip);

	if (ip == "")
	{
		ip = "127.0.0.1";
	}

	std::cout << "your Name: ";
	std::string name;
	getline(std::cin, name);

	client.Connect(ip.c_str(),name);
	client.Update();

	system("pause");
	return 0;
}
