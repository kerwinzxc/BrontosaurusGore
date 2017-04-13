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
	 void OpenDoor(const short aKeyId);

	 void LockDoor(const unsigned char aNetWorkID);
	 void LockDoor(const short aKeyId);

	 void UnlockDoor(const unsigned char aNetworkID);
	 void UnlockDoor(const short aKeyId);

	 void CloseDoor(const unsigned char aNetworkID);
	 void CloseDoor(const short aKeyId);
private:
	std::map<int, CDoorComponent*> myDoors;

	static CDoorManager* ourInstance;

	CDoorManager();
	~CDoorManager();
};

