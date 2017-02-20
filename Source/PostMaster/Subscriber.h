#pragma once

class Message;
enum class eMessageReturn;

class Subscriber
{
public:
	//Subscriber();
	Subscriber(int i);
	virtual ~Subscriber();

	virtual eMessageReturn Recieve(const Message& aMessage) = 0;
	
	inline void SetPriority(const int aPriority);
	inline int GetPriority() const;

private:
	int myPriority;
};

inline void Subscriber::SetPriority(const int aPriority)
{
	myPriority = aPriority;
}

inline int Subscriber::GetPriority() const
{
	return myPriority;
}
