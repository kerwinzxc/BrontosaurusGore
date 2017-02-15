#pragma once
#include <functional>   // std::bind
#include "../CommonUtilities/matrix.h"

#ifndef assert
#include <assert.h>
#endif

class CWwiseManager;

namespace Audio
{
	typedef unsigned int GameObjectID;

	class CAudioInterface
	{
	public:
		static CAudioInterface* GetInstance(){ return myInstance; }
		static void CreateInstance() { assert(myInstance == nullptr && "Audio interface already created"); myInstance = new CAudioInterface(); }
		static void Destroy(){ delete myInstance; myInstance = nullptr; }

		bool Init(const char* aInitBank);
		bool LoadBank(const char* aBankPath);
		void UnLoadBank(const char* aBankPath);

		void PostEvent(const char* aEvent, const Audio::GameObjectID aGameObjectID);
		void PostEvent(const char* aEvent);

		void Update();

		void SetListenerPosition(const CU::Matrix44f& aTranslation);
		void SetGameObjectPosition(const GameObjectID aGameObjectID, const CU::Vector3f& aPosition, const CU::Vector3f& aOrientation);

		void SetRTPCValue(const char* aRTPCName, const float aValue);

		//returns the ID of the registered gameobject
		Audio::GameObjectID RegisterGameObject();
		Audio::GameObjectID RegisterGameObject(const char* aGameObjectName);

		void UnregisterGameObject(const Audio::GameObjectID aGameObjectID);
		void UnregisterAllGameOBjects();


		typedef void(*callback_function)(const char*);
		void SetErrorCallBack(callback_function aErrorCallback);


	private:
		CAudioInterface();
		~CAudioInterface();

	private:
		static CAudioInterface* myInstance;

		Audio::GameObjectID myGameObjectCounter;
		Audio::GameObjectID myGameObjectID;

		CWwiseManager* myWwiseManager;
	};

}
