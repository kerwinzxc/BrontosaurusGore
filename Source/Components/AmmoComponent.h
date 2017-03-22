#pragma once
#include "Component.h"

struct SGeneralAmmoData; //Names are hard :/
class CAmmoComponent : public CComponent
{
public:
	CAmmoComponent();
	~CAmmoComponent();

	bool AmmoIndexValid() const;
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
	void Update(float aDeltaTime);
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;

private:
	void ChangeSelectedAmmoType(const char* aAmmoType);
private:
	CU::GrowingArray<SGeneralAmmoData*> myGeneralAmmoDataList;
	int mySelectedAmmoType;
};

