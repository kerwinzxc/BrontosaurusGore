#pragma once
#include <functional>
#include <map>
#include "KLoaderError.h"
//#include "../CommonUtilities/PJWrapper.h"
#include "../CommonUtilities/GrowingArray.h"

#include "../CommonUtilities/JsonValue.h"

namespace KLoader
{

	typedef int KId;

	struct LinkObject
	{
		LinkObject();

		int myIndex;

		CU::GrowingArray<KId> myChildren;
	};

	struct SLoadedComponentData
	{
		std::string myTypeName;
		CU::CJsonValue myData;
	};

	typedef std::function<int(SLoadedComponentData)> LoadFunction;
	typedef std::function<void(int, int)> LinkFunction;

	class CKevinLoader
	{
	public:
		static void CreateInstance();
		static CKevinLoader& GetInstance();
		static void DestroyInstance();

		void RegisterComponentLoadFunction(const std::string& ComponentType, const LoadFunction& aFunction);
		void RegisterComponentLinkFunction(const std::string& ComponentType, const LinkFunction& aFunction);

		void RegisterObjectLoadFunction(const LoadFunction& aFunction);
		void RegisterObjectLinkFunction(const LinkFunction& aLinkFunction);

		void ClearLinkObjectList();

		eError LoadFile(const std::string& aFilePath);

		std::string GetError();

		void PrintMissingComponents(const std::string& componentName);

	private:
		CKevinLoader();
		~CKevinLoader();

		int LoadComponent(const std::string& aIdString, const CU::CJsonValue& aRoot);

		eError LoadObjects(const CU::CJsonValue& aWrapper);
		eError LinkObjects();

		std::map<std::string, LoadFunction> myLoadFunctions;
		std::map<std::string, LinkFunction> myLinkFunctions;
		LoadFunction myObjectLoadFunction;
		LinkFunction myObjectLinkFunction;

		std::string myError;

		std::map<KId, LinkObject> myLinkObjects;
		
		static CKevinLoader* ourInstance;
		
#ifndef _RETAIL_BUILD
		bool myHasLoadedFile;
#endif
	};

	inline std::string CKevinLoader::GetError()
	{
		return myError;
	}

}
