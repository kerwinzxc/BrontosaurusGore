#include "stdafx.h"
#include "TShared_NetworkWrapper.h"
#include <iostream>
#include "../CommonUtilities/DL_Debug.h"
#include <string>
#include "NetworkMesssage_Connect.h"
#include "PackageType.h"
#include "NetworkMessage_Ping.h"
#include "NetworkMessage_PingResponse.h"

TShared_NetworkWrapper::TShared_NetworkWrapper() : myBuffer(nullptr), myCurrentBufferSize(0), mySocket(INVALID_SOCKET)
{
}


TShared_NetworkWrapper::~TShared_NetworkWrapper()
{
	closesocket(mySocket);
	WSACleanup();
}

bool TShared_NetworkWrapper::Init(unsigned short aPort)
{

	std::cout << "Initilizing winsock" << std::endl;

	INT iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &myWsaData);

	if (iResult != 0)
	{
		std::cout << "WSAStartup failed" << std::endl;
		std::cout << "MESSAGE :: " << gai_strerror(iResult) << std::endl;
		return false;
	}
	//createing socket
	std::cout << "Creating port" << std::endl;

	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (mySocket == INVALID_SOCKET)
	{
		std::cout << "Error creating socket:  " << WSAGetLastError() << std::endl;
		return false;
	}

	//bind the socket

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(aPort);

	iResult = bind(mySocket, reinterpret_cast<const sockaddr*>(&address), static_cast<int>(sizeof(sockaddr_in)));

	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Bind failed with error: " << WSAGetLastError() << std::endl;
		return false;
	}

	DWORD nonBlocking = 1;
	iResult = ioctlsocket(mySocket, FIONBIO, &nonBlocking);

	if (iResult != 0)
	{
		DL_PRINT("faild to set nonBlocking");
		return false;
	}

	return true;
}

bool TShared_NetworkWrapper::Send(CNetworkMessage* aNetworkMessage, const char* aRecieverAdress, const char* aRecieverPort)
{
	aNetworkMessage->PackMessage();
	const StreamType& streamType = aNetworkMessage->GetSerializedData();

	const unsigned tempBufferSize = aNetworkMessage->GetSerializedData().size();
	SNetworkPackageHeader aHeader = aNetworkMessage->GetHeader();

	unsigned currentPacketSize = sizeof(SNetworkPackageHeader) + streamType.size();

	if (myCurrentBufferSize < currentPacketSize)
	{
		if (myBuffer != nullptr)
		{
			delete myBuffer;
		}
		myCurrentBufferSize = currentPacketSize;
		myBuffer = new char[myCurrentBufferSize];
	}

	memcpy_s(myBuffer, myCurrentBufferSize, &aNetworkMessage->GetHeader(), sizeof(SNetworkPackageHeader));
	memcpy_s(&myBuffer[sizeof(SNetworkPackageHeader)], myCurrentBufferSize - sizeof(SNetworkPackageHeader), streamType.data(), streamType.size());

	sockaddr_in adress;
	adress.sin_family = AF_INET;
	adress.sin_port = htons(std::stoi(aRecieverPort));

	inet_pton(AF_INET, aRecieverAdress, &(adress.sin_addr));

	const int bytesSent = sendto(mySocket, myBuffer, currentPacketSize, 0, reinterpret_cast<sockaddr*>(&adress), sizeof(adress));

	if (bytesSent != currentPacketSize)
	{
		std::cout << "Failed to send packet";
		std::string error;
		error += WSAGetLastError();
		std::cout << error.c_str();
	}

	return true;
}

bool TShared_NetworkWrapper::Send(SNetworkPackageHeader aPackageHeader, const char* aData, unsigned aDataSize, const char* aRecieverAdress, const char* aRecieverPort)
{
	unsigned currentPacketSize = sizeof(aPackageHeader) + aDataSize;

	if (myCurrentBufferSize < currentPacketSize)
	{
		if (myBuffer != nullptr)
		{
			delete myBuffer;
		}
		myCurrentBufferSize = currentPacketSize;
		myBuffer = new char[myCurrentBufferSize];

	}

	memcpy_s(myBuffer, myCurrentBufferSize, &aPackageHeader, sizeof(aPackageHeader));
	memcpy_s(&myBuffer[sizeof(aPackageHeader)], myCurrentBufferSize - sizeof(SNetworkPackageHeader), aData, aDataSize);

	sockaddr_in adress;
	adress.sin_family = AF_INET;
	adress.sin_port = htons(std::stoi(aRecieverPort));

	inet_pton(AF_INET, aRecieverAdress, &(adress.sin_addr));

	const int bytesSent = sendto(mySocket, myBuffer, currentPacketSize, 0, reinterpret_cast<sockaddr*>(&adress), sizeof(adress));

	if (bytesSent != currentPacketSize)
	{
		std::cout << "Failed to send packet";
		std::string error;
		error += WSAGetLastError();
		std::cout << error.c_str();
	}

	return true;
}

CNetworkMessage* TShared_NetworkWrapper::Recieve(char** senderIp, char** senderPort)
{
	unsigned char buffer[MAX_PACKET_SIZE];

	sockaddr_in fromAddress;
	int addressSize = sizeof(fromAddress);

	const int bytes = recvfrom(mySocket, reinterpret_cast<char*>(buffer), MAX_PACKET_SIZE, 0, reinterpret_cast<sockaddr*>(&fromAddress), &addressSize);

	if (bytes <= 0)
	{
		return new CNetworkMessage;
	}

	if (senderIp != nullptr)
	{
		*senderIp = new char[INET_ADDRSTRLEN];
		inet_ntop(fromAddress.sin_family, &(fromAddress.sin_addr), *senderIp, INET_ADDRSTRLEN);
	}

	if (senderPort != nullptr)
	{
		std::string port;
		port += std::to_string(ntohs(fromAddress.sin_port));
		
		*senderPort = new char[port.size() + 1];
		memcpy_s(*senderPort, port.size() + 1, port.c_str(), port.size() + 1);
	}

	SNetworkPackageHeader header;

	memcpy_s(&header, sizeof(header), buffer, sizeof(header));

	const unsigned dataSize = bytes - sizeof(header);

	char * data = new char[dataSize];

	memcpy_s(data, dataSize, &buffer[sizeof(header)], dataSize);

	CNetworkMessage* newMessage = new CNetworkMessage();

	newMessage->SetData(data, dataSize);
	newMessage->SetHeader(header);

	delete[] data;
	return  newMessage;
}
