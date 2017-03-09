#include "stdafx.h"
#include "PhysXAllocator.h"
#include "foundation/PxAssert.h"

namespace Physics
{
	CPhysXAllocator::CPhysXAllocator()
	{
	}


	CPhysXAllocator::~CPhysXAllocator()
	{
	}

	void * CPhysXAllocator::allocate(size_t size, const char * typeName, const char * filename, int line)
	{
		void* ptr = platformAlignedAlloc(size);
		//PHYSICS_LOG("%s (%i Bytes) allocated at line: %i, in file: %s ", typeName, (int)size, line, filename);
		PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15) == 0);
		return ptr;
	}

	void CPhysXAllocator::deallocate(void * ptr)
	{
		platformAlignedFree(ptr);
	}
}
