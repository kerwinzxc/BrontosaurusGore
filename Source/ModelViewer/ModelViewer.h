#pragma once
#include "../PostMaster/Subscriber.h"

class CScene;

typedef unsigned int InstanceID;

namespace CU
{
	class Time;
}

class CModelViewer : public Subscriber
{
public:
	CModelViewer();
	~CModelViewer();

	void Init(const char aStartModelPath[]);
	void Update(const CU::Time& aDeltaTime);
	void Render();

	void LoadModel(const std::string& aModelPath);

private:
	eMessageReturn Recieve(const Message& aMessage) override;
	
	CScene* myScene;
	InstanceID myCurrentModel;
};
