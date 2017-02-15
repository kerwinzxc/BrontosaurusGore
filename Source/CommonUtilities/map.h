#pragma once
#include "assert.h"

#define MAP_TEMPLATE_ARGS template<typename KEY, typename VALUE>


#ifdef __APPLE__
	#define SPRINTF sprintf
#elif defined _WIN32 || defined _WIN64
	#define SPRINTF sprintf_s
#endif


namespace CommonUtilities
{
    template<typename KEY, typename VALUE>;
    class Map

    {
    public:
        
        void Delete(const KEY& aKey); // assertar om keyn inte finns

        bool KeyExists(const KEY& aKey);

        VALUE& operator[](const KEY& aKey); // Ska skapa en key om den inte redan existerar
        
    private:
    
    
    };
}