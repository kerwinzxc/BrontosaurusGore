#include "WwiseManager.h"
#include <string>

#define DEMO_DEFAULT_POOL_SIZE 2*1024*1024
#define DEMO_LENGINE_DEFAULT_POOL_SIZE 2*1024*1024

namespace AK
{
	void * AllocHook(size_t in_size)
	{
		return malloc(in_size);
	}
	void FreeHook(void * in_ptr)
	{
		free(in_ptr);
	}
	void * VirtualAllocHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwAllocationType,
		DWORD in_dwProtect
		)
	{
		return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
	}
	void VirtualFreeHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwFreeType
		)
	{
		VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
	}
}

namespace AK_HELPER
{
	/// Returns the number of elements in an array
	template <class T, size_t N> AkForceInline size_t AK_ARRAYSIZE(T(&)[N])
	{
		return N;
	}
}

CAkFilePackageLowLevelIOBlocking* m_pLowLevelIO;
CWwiseManager::CWwiseManager()
	:myErrorCallbck(nullptr)
{
}


CWwiseManager::~CWwiseManager()
{
	AK::SoundEngine::StopAll();
	UnregisterAllGameObjects();
	AK::SoundEngine::ClearBanks();
	TermWwise();
}

bool CWwiseManager::Init(const char* aInitBank)
{
	AkMemSettings memSettings;
	AkStreamMgrSettings stmSettings;
	AkDeviceSettings deviceSettings;
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AkMusicSettings musicInit;

	memSettings.uMaxNumPools = 20;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	initSettings.uDefaultPoolSize = DEMO_DEFAULT_POOL_SIZE;

	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
	platformInitSettings.uLEngineDefaultPoolSize = DEMO_LENGINE_DEFAULT_POOL_SIZE;

	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	platformInitSettings.hWnd = GetActiveWindow();

	// Initialize Wwise
	AkOSChar szError[500];
	bool bSuccess = InitWwise(memSettings, stmSettings, deviceSettings, initSettings, platformInitSettings, musicInit, szError, (unsigned int)AK_HELPER::AK_ARRAYSIZE(szError));
	if (!bSuccess)
	{
		return false;
	}

	AkBankID bankIDInit;
	if (AK::SoundEngine::LoadBank(aInitBank, AK_DEFAULT_POOL_ID, bankIDInit) != AK_Success)
	{
		return false;
	}

	return true;
}

void CWwiseManager::Update()
{
	AK::SoundEngine::RenderAudio();
}


bool CWwiseManager::LoadBank(const char* aBankPath)
{
	AkBankID bankID; // Not used
	AKRESULT result = AK::SoundEngine::LoadBank(aBankPath, AK_DEFAULT_POOL_ID, bankID);
	ErrorData errorData;
	errorData.path = aBankPath;
	return ErrorHandling(result, errorData);
}


void CWwiseManager::UnLoadBank(const char* aBankPath)
{
	AK::SoundEngine::UnloadBank(aBankPath, nullptr);
}


void CWwiseManager::PostEvent(const char* aEvent, const AkGameObjectID aGameObjectID)
{
	AkPlayingID result = AK::SoundEngine::PostEvent(aEvent, aGameObjectID);
	//AK::SoundEngine::SetRTPCValue("engine", 1,)
}

void CWwiseManager::SetRTPC(const char * aRTPCName, const float aValue)
{
	AK::SoundEngine::SetRTPCValue(aRTPCName, aValue);
}

bool CWwiseManager::InitWwise(AkMemSettings &in_memSettings, AkStreamMgrSettings &in_stmSettings, AkDeviceSettings &in_deviceSettings, AkInitSettings &in_initSettings, AkPlatformInitSettings &in_platformInitSettings, AkMusicSettings &in_musicInit, AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount)
{
	m_pLowLevelIO = new CAkFilePackageLowLevelIOBlocking();
	//
	// Create and initialize an instance of the default memory manager. Note
	// that you can override the default memory manager with your own. Refer
	// to the SDK documentation for more information.
	//

	AKRESULT res = AK::MemoryMgr::Init(&in_memSettings);
	if (res != AK_Success)
	{
		CallError("Momory manager could not init!");
		return false;
	}

	//
	// Create and initialize an instance of the default streaming manager. Note
	// that you can override the default streaming manager with your own. Refer
	// to the SDK documentation for more information.
	//

	// Customize the Stream Manager settings here.

	if (!AK::StreamMgr::Create(in_stmSettings))
	{
		CallError("Stream manager could not init!");
		return false;
	}

	// 
	// Create a streaming device with blocking low-level I/O handshaking.
	// Note that you can override the default low-level I/O module with your own. Refer
	// to the SDK documentation for more information.        
	//

	// CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
	// in the Stream Manager, and registers itself as the File Location Resolver.
	res = m_pLowLevelIO->Init(in_deviceSettings);
	if (res != AK_Success)
	{
		CallError("Low level IO could not init!");
		return false;
	}

	//
	// Create the Sound Engine
	// Using default initialization parameters
	//

	res = AK::SoundEngine::Init(&in_initSettings, &in_platformInitSettings);
	if (res != AK_Success)
	{
		CallError("Sound engine could not init!");
		return false;
	}

	//
	// Initialize the music engine
	// Using default initialization parameters
	//

	res = AK::MusicEngine::Init(&in_musicInit);
	if (res != AK_Success)
	{
		CallError("Music engine could not init!");
		return false;
	}

#ifdef _DEBUG
#if !defined AK_OPTIMIZED && !defined INTEGRATIONDEMO_DISABLECOMM
	//
	// Initialize communications (not in release build!)
	//
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);
	res = AK::Comm::Init(commSettings);
	if (res != AK_Success)
	{
		CallError("AkCommSettings error!");
	}
#endif // AK_OPTIMIZED
#endif

	//
	// Register plugins
	/// Note: This a convenience method for rapid prototyping. 
	/// To reduce executable code size register/link only the plug-ins required by your game 
	res = AK::SoundEngine::RegisterAllPlugins();
	if (res != AK_Success)
	{
		CallError("Plugins could not register!");
	}

	return true;
}

bool CWwiseManager::ErrorHandling(AKRESULT aResult, const ::CWwiseManager::ErrorData& aErrorData)
{
	if (aResult != AK_Success)
	{
		switch (aResult)
		{
		case AK_FileNotFound:
		{
			std::string error = "File \"";
			error += aErrorData.path;
			error += "\" could not be found.";
			CallError(error.c_str());
		}
		break;

		case AK_WrongBankVersion:
		{
			std::string error = "File \"";
			error += aErrorData.path;
			error += "\" is of the wrong version.";
			CallError(error.c_str());
		}
			break;
		case AK_Success: break;
		case AK_Fail: 
		case AK_NotImplemented:
		case AK_PartialSuccess:
		case AK_NotCompatible:
		case AK_AlreadyConnected:
		case AK_NameNotSet:
		case AK_InvalidFile:
		case AK_AudioFileHeaderTooLarge:
		case AK_MaxReached:
		case AK_InputsInUsed:
		case AK_OutputsInUsed:
		case AK_InvalidName:
		case AK_NameAlreadyInUse:
		case AK_InvalidID:
		case AK_IDNotFound:
		case AK_InvalidInstanceID:
		case AK_NoMoreData:
		case AK_NoSourceAvailable:
		case AK_StateGroupAlreadyExists:
		case AK_InvalidStateGroup:
		case AK_ChildAlreadyHasAParent:
		case AK_InvalidLanguage:
		case AK_CannotAddItseflAsAChild:
		case AK_UserNotInList:
		case AK_NoTransitionPoint:
		case AK_InvalidParameter:
		case AK_ParameterAdjusted:
		case AK_IsA3DSound:
		case AK_NotA3DSound:
		case AK_ElementAlreadyInList:
		case AK_PathNotFound:
		case AK_PathNoVertices:
		case AK_PathNotRunning:
		case AK_PathNotPaused:
		case AK_PathNodeAlreadyInList:
		case AK_PathNodeNotInList:
		case AK_VoiceNotFound:
		case AK_DataNeeded:
		case AK_NoDataNeeded:
		case AK_DataReady:
		case AK_NoDataReady:
		case AK_NoMoreSlotAvailable:
		case AK_SlotNotFound:
		case AK_ProcessingOnly:
		case AK_MemoryLeak:
		case AK_CorruptedBlockList:
		case AK_InsufficientMemory:
		case AK_Cancelled:
		case AK_UnknownBankID:
		case AK_IsProcessing:
		case AK_BankReadError:
		case AK_InvalidSwitchType:
		case AK_VoiceDone:
		case AK_UnknownEnvironment:
		case AK_EnvironmentInUse:
		case AK_UnknownObject:
		case AK_NoConversionNeeded:
		case AK_FormatNotReady:
		case AK_DataReadyNoProcess:
		case AK_DeviceNotReady:
		case AK_CouldNotCreateSecBuffer:
		case AK_BankAlreadyLoaded:
		case AK_RenderedFX:
		case AK_ProcessNeeded:
		case AK_ProcessDone:
		case AK_MemManagerNotInitialized:
		case AK_StreamMgrNotInitialized:
		case AK_SSEInstructionsNotSupported:
		case AK_Busy:
		case AK_UnsupportedChannelConfig:
		case AK_PluginMediaNotAvailable:
		case AK_MustBeVirtualized:
		case AK_CommandTooLarge:
		case AK_RejectedByFilter:
		case AK_InvalidCustomPlatformName:
		case AK_DLLCannotLoad:
		default: break;
		}

		char buffer[50];
		sprintf_s(buffer, "Unhandled wwise error in %s!", aErrorData.path.c_str());

		CallError(buffer);
		//SetLoadFileErrorMessage("Positioning_Demo.bnk");
		return false;
	}
	return true;
}

void CWwiseManager::TermWwise()
{
#ifdef _DEBUG
#if !defined AK_OPTIMIZED && !defined INTEGRATIONDEMO_DISABLECOMM
	// Terminate communications between Wwise and the game
	AK::Comm::Term();
#endif // AK_OPTIMIZED
#endif
	// Terminate the music engine
	AK::MusicEngine::Term();

	// Terminate the sound engine
	if (AK::SoundEngine::IsInitialized())
	{
		AK::SoundEngine::Term();
	}

	// Terminate the streaming device and streaming manager
	// CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming device 
	// that lives in the Stream Manager, and unregisters itself as the File Location Resolver.
	if (AK::IAkStreamMgr::Get())
	{
		m_pLowLevelIO->Term();
		AK::IAkStreamMgr::Get()->Destroy();
	}

	// Terminate the Memory Manager
	if (AK::MemoryMgr::IsInitialized())
	{
		AK::MemoryMgr::Term();
	}
}

void CWwiseManager::SetErrorCallBack(callback_function aErrorCallback)
{
	myErrorCallbck = aErrorCallback;
}

void CWwiseManager::SetListenerPosition(const AkListenerPosition & aListenerPosition)
{
	AK::SoundEngine::SetListenerPosition(aListenerPosition);
}

void CWwiseManager::SetGameObjectPosition(const AkGameObjectID aGameObjectID, const AkSoundPosition & aSoundPosition)
{
	AK::SoundEngine::SetPosition(aGameObjectID, aSoundPosition);
	
}

void CWwiseManager::RegisterGameObject(const AkGameObjectID aGameObjectID)
{
	AK::SoundEngine::RegisterGameObj(aGameObjectID);
}

void CWwiseManager::RegisterGameObject(const AkGameObjectID aGameObjectID, const char * aGameObjectName)
{
	AK::SoundEngine::RegisterGameObj(aGameObjectID, aGameObjectName);
}

void CWwiseManager::UnregisterGameObject(const AkGameObjectID aGameObjectID)
{
	AK::SoundEngine::UnregisterGameObj(aGameObjectID);
}

void CWwiseManager::UnregisterAllGameObjects()
{
	AK::SoundEngine::UnregisterAllGameObj();
}

void CWwiseManager::CallError(const char* aError)
{
	if (myErrorCallbck)
	{
		myErrorCallbck(aError);
	}
}