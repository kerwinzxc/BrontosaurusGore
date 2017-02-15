#pragma once

void DumpMemoryLeeks();

//#define WATCH_THE_MEMORY_MKAY

#ifdef WATCH_THE_MEMORY_MKAY
#ifndef _RETAIL_BUILD

void* operator new(size_t aSize, const char* aFileName, const int aLine);
void operator delete(void* aPtr);


#define CARL_NEW new(__FILE__, __LINE__)
#undef new
#define new CARL_NEW

#endif // !_RETAIL_BUILD

#endif // WATCH_THE_MEMORY_MKAY
