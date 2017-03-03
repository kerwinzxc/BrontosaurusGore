#pragma once
#include "ResourceCallback.h"

namespace Physics
{
	namespace Apex
	{
		class CApexResourceCallback : public nvidia::apex::ResourceCallback
		{
		public:
			CApexResourceCallback();
			~CApexResourceCallback();
			void* requestResource(const char* nameSpace, const char* name) override;
			void releaseResource(const char* nameSpace, const char* name, void* resource) override;
		};
	}
}

