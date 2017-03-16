#pragma once
#include "Component.h"

class CEnemyClientRepresentation : public CComponent
{
public:
	CEnemyClientRepresentation(unsigned int anId);
	~CEnemyClientRepresentation();
protected:
	unsigned myComponentId;
};

