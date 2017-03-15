#include "stdafx.h"
#include "KevinLoader.h"
#include <fstream>
#include "../Components/ComponentManager.h"
#include "../LoadManager/LoadManager.h"
#include "../Components/GameObjectManager.h"

namespace KLoader
{
	CKevinLoader* CKevinLoader::ourInstance = nullptr;

	LinkObject::LinkObject(): myIndex(0)
	{
		myChildren.Init(1);
	}

	void CKevinLoader::CreateInstance()
	{
		if (ourInstance == nullptr)
		{
			ourInstance = new CKevinLoader;
		}
	}

	CKevinLoader& CKevinLoader::GetInstance()
	{
		if (ourInstance == nullptr)
		{
			DL_ASSERT("CKevinLoader(JsonImporter) has not been created");
		}

		return *ourInstance;
	}

	void CKevinLoader::DestroyInstance()
	{
		if (ourInstance != nullptr)
		{
			delete ourInstance;
			ourInstance = nullptr;
		}
	}

	void CKevinLoader::RegisterComponentLoadFunction(const std::string& ComponentType, const LoadFunction& aFunction)
	{
		myLoadFunctions[ComponentType] = aFunction;
	}

	void CKevinLoader::RegisterComponentLinkFunction(const std::string& ComponentType, const LinkFunction& aFunction)
	{
		myLinkFunctions[ComponentType] = aFunction;
	}

	void CKevinLoader::RegisterObjectLoadFunction(const LoadFunction& aFunction)
	{
		myObjectLoadFunction = aFunction;
	}

	void CKevinLoader::RegisterObjectLinkFunction(const LinkFunction& aLinkFunction)
	{
		myObjectLinkFunction = aLinkFunction;
	}

	void CKevinLoader::ClearLinkObjectList()
	{
		myLinkObjects.clear();
	}

	eError CKevinLoader::LoadFile(const std::string& aFilePath)
	{
		CU::CJsonValue wrapper;
		std::string mybeKeepThishshould = wrapper.Parse(aFilePath);
		if (!mybeKeepThishshould.empty())
		{
			DL_MESSAGE_BOX("Error loading level %s\n%s", aFilePath.c_str(), mybeKeepThishshould.c_str());
		}

		const eError loadObjectError = LoadObjects(wrapper);
		if (loadObjectError != eError::NO_LOADER_ERROR)
		{
			return loadObjectError;
		}

		const eError linkObjectError = LinkObjects();
		if (linkObjectError != eError::NO_LOADER_ERROR)
		{
			return linkObjectError;
		}

		ClearLinkObjectList();

		return eError::NO_LOADER_ERROR;
		
	}

	void CKevinLoader::PrintMissingComponents(const std::string& aComponentName)
	{
	}

	CKevinLoader::CKevinLoader()
	{
#ifndef _RETAIL_BUILD
		myHasLoadedFile = false;
#endif
	}

	CKevinLoader::~CKevinLoader()
	{
	}

	int CKevinLoader::LoadComponent(const std::string& aIdString, const CU::CJsonValue& aRoot)
	{
		//const CU::JsonObject rootObject = aRoot.GetJsonObject();
		const CU::CJsonValue componentsObject = aRoot["components"];

		if (componentsObject.HasKey(aIdString) == false)
		{
			return 0;
		}

		const CU::CJsonValue currentComponent = componentsObject[aIdString];
		const std::string& type = currentComponent["type"].GetString();
		if (myLoadFunctions.count(type) == 0)
		{
			PrintMissingComponents(type);
			return 0;
		}

		SLoadedComponentData componentData;

		componentData.myTypeName = type;
		componentData.myData = currentComponent["data"];

		return myLoadFunctions[type](componentData);
	}

	eError CKevinLoader::LoadObjects(const CU::CJsonValue& aRoot)
	{
		//CU::JsonObject rootObject = aRoot.GetJsonObject();

		if (aRoot.HasKey("objects") == false || aRoot["objects"].IsArray() == false )
		{
			myError = "LevelData json document missing objects array";
			return eError::MISSING_OBJECTS;
		}

		const CU::CJsonValue objectsArray = aRoot["objects"];

		for (int i = 0; i < objectsArray.Size(); ++i)
		{
			CU::CJsonValue currentObject = objectsArray[i];
			SLoadedComponentData componentData;

			componentData.myTypeName = currentObject["name"].GetString();
			componentData.myData = currentObject;

			const KId currentID = currentObject["id"].GetInt() * -1;

			LinkObject currentLink;

			if (myLinkObjects.count(currentID) > 0)
			{
				currentLink = myLinkObjects[currentID];
			}
			
			if (currentObject.HasKey("parent") == true)
			{
				const CU::CJsonValue parentIDObj = currentObject["parent"];
				const KId parentId = parentIDObj.GetInt() * -1;
				if (myLinkObjects.count(parentId) == 0)
				{
					myLinkObjects[parentId] = LinkObject();
				}

				myLinkObjects[parentId].myChildren.Add(currentID);
			}

			if (!myObjectLoadFunction)
			{
				return eError::MISSING_OBJECT_LOAD_FUNC;
			}

			 currentLink.myIndex = myObjectLoadFunction(componentData);
			 myCurrentObjectIndex = currentLink.myIndex;
			 const CU::CJsonValue componentsArray = currentObject["components"];
			 for (int j = 0; j < componentsArray.Size(); ++j)
			 {
				 const std::string& IdString = componentsArray[j].GetString();

				 const int componentId = LoadComponent(IdString, aRoot);

				 if (componentId != 0)
				 {
					 myObjectLinkFunction(currentLink.myIndex, componentId);
				 }
			 }

			 myLinkObjects[currentID] = currentLink;


		}
		return eError::NO_LOADER_ERROR;
	}

	eError CKevinLoader::LinkObjects()
	{
		for (auto linkObject : myLinkObjects)
		{
			const LinkObject& currentLink = linkObject.second;

			if (currentLink.myChildren.Size() > 0)
			{
				for (unsigned int i = 0; i < currentLink.myChildren.Size(); ++i)
				{
					if (myObjectLinkFunction)
					{
						myObjectLinkFunction(currentLink.myIndex, myLinkObjects[currentLink.myChildren[i]].myIndex);
					}
					else
					{
						return eError::MISSING_OBJECT_LINK_FUNC;
					}
				}
			}
		}

		return eError::NO_LOADER_ERROR;
	}
}
