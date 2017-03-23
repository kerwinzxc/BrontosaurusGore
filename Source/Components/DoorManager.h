#pragma once
typedef short lockID;

class CDoorComponent;

class CDoorManager
{
public:

	static void Create();
	static void Destroy();
	static CDoorManager* GetInstance();

	void Update(const CU::Time& aDeltaTime);

	 CDoorComponent* CreateDoorComponent(const CU::Vector2f& aOpenDirection,const bool aIsLocked, const lockID aLockId, const bool aIsClosed, const bool aShouldResetOnPlayerDeath);

	 void OpenDoor(const unsigned char aNetworkID);
	 void UnlockDoor(const unsigned char aNetworkID);
	 void CloseDoor(const unsigned char aNetworkID);
private:
	std::map<int, CDoorComponent*> myDoors;

	static CDoorManager* ourInstance;

	CDoorManager();
	~CDoorManager();
};

