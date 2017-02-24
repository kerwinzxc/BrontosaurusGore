#pragma once
#include "../PostMaster/Subscriber.h"
#include "../ThreadedPostmaster/Subscriber.h"

class CScene;

typedef unsigned int InstanceID;

namespace CU
{
	class Time;
}

class CModelViewer : public Postmaster::ISubscriber
{
public:
	CModelViewer();
	~CModelViewer();

	void Init(const char aStartModelPath[]);
	void Update(const CU::Time& aDeltaTime);
	void Render();

	void LoadModel(const std::string& aModelPath);

	eMessageReturn DoEvent(const DroppedFile& aDroppedFile) override;
private:
	
	CScene* myScene;
	InstanceID myCurrentModel;
};
