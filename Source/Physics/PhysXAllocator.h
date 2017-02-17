#pragma once
#include "foundation/PxAllocatorCallback.h"
#include "common/PxPhysXCommonConfig.h"
#include <stdlib.h>
#include <malloc.h>



class CPhysXAllocator : public physx::PxAllocatorCallback
{
public:
	CPhysXAllocator();
	virtual ~CPhysXAllocator();



	/**
	\brief Allocates size bytes of memory, which must be 16-byte aligned.

	This method should never return NULL.  If you run out of memory, then
	you should terminate the app or take some other appropriate action.

	<b>Threading:</b> This function should be thread safe as it can be called in the context of the user thread
	and physics processing thread(s).

	\param size			Number of bytes to allocate.
	\param typeName		Name of the datatype that is being allocated
	\param filename		The source file which allocated the memory
	\param line			The source line which allocated the memory
	\return				The allocated block of memory.
	*/
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line);
	/**
	\brief Frees memory previously allocated by allocate().

	<b>Threading:</b> This function should be thread safe as it can be called in the context of the user thread
	and physics processing thread(s).

	\param ptr Memory to free.
	*/
	virtual void deallocate(void* ptr) override;



private:

	// on win32 we only have 8-byte alignment guaranteed, but the CRT provides special aligned allocation fns
	PX_FORCE_INLINE void* platformAlignedAlloc(size_t size)
	{
		return _aligned_malloc(size, 16);
	}

	PX_FORCE_INLINE void platformAlignedFree(void* ptr)
	{
		_aligned_free(ptr);
	}

};


