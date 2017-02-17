#pragma once
#include <string>
#include <atomic>
#include <thread>

class CChat
{
public:
	CChat();
	~CChat();

	void StartChat();
	void StopChat();

	void Update();
	std::string GetChatMessage();

private:
	std::string myCurrentMessage;
	std::atomic<bool> myWaiting;
	std::atomic<bool> myIsRunnning;

	std::thread* myThread;
};

