#pragma once
#include "Component.h"
class CEnemyRunTowardsComponent : public CComponent
{
public:
	CEnemyRunTowardsComponent();
	~CEnemyRunTowardsComponent();

	inline const bool GetIsOccupied() const;
	inline void SetIsOccupied(bool aBool);
private:
	bool myIsOccupied;
};

inline const bool CEnemyRunTowardsComponent::GetIsOccupied() const
{
	return myIsOccupied;
}
inline void CEnemyRunTowardsComponent::SetIsOccupied(bool aIsOccupied)
{
	myIsOccupied = aIsOccupied;
}