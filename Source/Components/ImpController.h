#pragma once
#include "Enemy.h"
class CImpController : public CEnemy
{
public:
	CImpController(unsigned int aId);
	~CImpController();

	void Update(const float aDeltaTime) override;
private:

};

