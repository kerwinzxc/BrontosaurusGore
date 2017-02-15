#pragma once

#include "../CommonUtilities/stack.h"

using FireEmitterID = int;

class CFireEmitter;
struct SFireEmitterData;

class CFireEmitterManager
{
public:
	friend class CEngine;

	FireEmitterID CreateFireEmitter(const SFireEmitterData& aFireEmitterData);
	int CopyFireEmitter(const FireEmitterID aFireEmitter);
	int DestroyFireEmitter(const FireEmitterID aFireEmitter);

	CFireEmitter* GetFireEmitter(const FireEmitterID aID);

private:
	CFireEmitterManager();
	~CFireEmitterManager();

	CU::GrowingArray<CFireEmitter, FireEmitterID> myFireEmitters;
	CU::Stack<FireEmitterID> myFreeIDs;
};
