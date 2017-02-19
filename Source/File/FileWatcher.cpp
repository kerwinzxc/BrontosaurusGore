#include "FileWatcher.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <tchar.h>
#include <minwinbase.h>

System::File::CFileWatcher::CFileWatcher(): CFileWatcher(1000)
{
}

System::File::CFileWatcher::CFileWatcher(unsigned aBufferSize) : myIsRunning(true), myRunThread(nullptr)
{
	
}

System::File::CFileWatcher::~CFileWatcher()
{
	Stop();
}

void System::File::CFileWatcher::Start()
{
	myRunThread = new std::thread(&System::File::CFileWatcher::Run, this);
}

void System::File::CFileWatcher::Stop()
{
	myIsRunning = false;
	if(myRunThread != nullptr)
	{
		myRunThread->join();
		delete myRunThread;
		myRunThread = nullptr;
	}
}

void System::File::CFileWatcher::AddFile(const std::string& aFilePath)
{

	AddFile(std::wstring(aFilePath.begin(), aFilePath.end()));
}

void System::File::CFileWatcher::AddFile(const std::wstring& aFilePath)
{
	WatchData watchData = ParseWatchdata(aFilePath);
	myAddFileQueue.Push(watchData);
	myWaitForFile.notify_one();
}

void System::File::CFileWatcher::RemoveFile(const std::string& aFilePath)
{

	RemoveFile(std::wstring(aFilePath.begin(), aFilePath.end()));
}

void System::File::CFileWatcher::RemoveFile(const std::wstring& aFilePath)
{

	myRemoveFileQueue.Push(aFilePath);
}

void System::File::CFileWatcher::HandleWriteChange(HANDLE aDirHandle, const WatchDataCollection& aWatchData) const
{
	static const int NumberOfInfo = 1024;
	FILE_NOTIFY_INFORMATION buffer[NumberOfInfo];
	ZeroMemory(&buffer, sizeof(buffer));

	DWORD bytes = 0;
	const bool result = ReadDirectoryChangesW(aDirHandle, &buffer, sizeof(buffer) , FALSE,
		FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_FILE_NAME, &bytes, NULL, NULL);
	if(result == false)
	{
		DWORD error = GetLastError();
		
		return;
	}

	FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[0]);

	std::wstring fileName = fni->FileName;
	for(int i = 0; i < aWatchData.Size(); ++i)
	{
		const WatchData& data = aWatchData[i];

		if(data.fileName == fileName)
		{
			NotifyFileChange(data);
			break;
		}
	}

	FindNextChangeNotification(aDirHandle);
}


void System::File::CFileWatcher::WatchChange()
{
	//TODO: Implement file watching here

	for(std::map<HANDLE, WatchDataCollection>::iterator it = myWatchData.begin(); it != myWatchData.end(); ++it)
	{
		DWORD waitStatus = WaitForSingleObject(it->first,10);

		switch (waitStatus)
		{
		case WAIT_OBJECT_0:
			HandleWriteChange(it->first,it->second);
			break;

		case WAIT_TIMEOUT:

			break;
		default: 
			break;
		}
	}

}

void System::File::CFileWatcher::Run()
{
	while (myIsRunning == true)
	{
		RemoveFiles();
		GetNewFiles();
		WaitForFile();
		WatchChange();
		std::this_thread::yield();
	}
}

void System::File::CFileWatcher::WaitForFile()
{
	std::unique_lock<std::mutex> waitLock(myFileWaitLock);

	myWaitForFile.wait(waitLock, [this] {return myWatchData.size() != 0  || myAddFileQueue.IsEmpty() == false; });
}


System::File::CFileWatcher::WatchData System::File::CFileWatcher::ParseWatchdata(const std::wstring& aFilePath)
{
	const int lastSlash = aFilePath.find_last_of(L"/\\");

	WatchData watchData;
	if(lastSlash == aFilePath.npos)
	{
		watchData.fileName = aFilePath;
		return watchData;
	}

	watchData.dirPath = aFilePath.substr(0, lastSlash);
	watchData.fileName = aFilePath.substr(lastSlash + 1, aFilePath.size() - lastSlash);
	return watchData;
}

void System::File::CFileWatcher::RemoveFiles()
{
	while (myRemoveFileQueue.IsEmpty() == false)
	{
		const std::wstring removeFile = myRemoveFileQueue.Pop();
		const WatchData watchData = ParseWatchdata(removeFile);
		
		std::map<std::wstring, HANDLE>::iterator handleIt = myDirHandles.find(watchData.dirPath);
		if(handleIt != myDirHandles.end())
		{
			HANDLE handle = handleIt->second;

			std::map<HANDLE, WatchDataCollection>::iterator collectionIt = myWatchData.find(handle);

			if(collectionIt != myWatchData.end())
			{
				CU::GrowingArray<WatchData>& collection = collectionIt->second;

				for(int i = 0; i < collection.Size(); ++i)
				{
					const WatchData& data = collection[i];
					if(data.fileName == watchData.fileName)
					{
						collection.RemoveCyclicAtIndex(i);
						break;
					}
				}
				if(collection.Empty() == true)
				{
					myWatchData.erase(collectionIt);
					myDirHandles.erase(handleIt);
				}
			}
			else
			{
				myDirHandles.erase(handleIt);
			}
		}
	}
}


HANDLE System::File::CFileWatcher::GetDirHandle(const std::wstring& aDirPath)
{
	std::map<std::wstring, HANDLE>::iterator handleIt = myDirHandles.find(aDirPath);

	if(handleIt == myDirHandles.end())
	{
		const HANDLE handle = FindFirstChangeNotificationW(
			aDirPath.c_str(),
			FALSE,
			FILE_NOTIFY_CHANGE_LAST_WRITE);

		myDirHandles[aDirPath] = handle;
		myWatchData[handle].Init(10);

		return handle;
	}

	return handleIt->second;
}

void System::File::CFileWatcher::CreateWatchData(WatchData aAddFile)
{
	DWORD waitStatus;
	HANDLE changeHandle = NULL;

	changeHandle = GetDirHandle(aAddFile.dirPath);

	if(changeHandle == INVALID_HANDLE_VALUE || changeHandle == NULL)
	{
		DWORD error = GetLastError();
		
		return;
	}

	aAddFile.watchHandle = changeHandle;
	myWatchData[changeHandle].Add(aAddFile);
}

bool System::File::CFileWatcher::InCollection(const WatchData& aWatchData)
{
	std::map<std::wstring, HANDLE>::iterator handleIt = myDirHandles.find(aWatchData.dirPath);

	if (handleIt != myDirHandles.end())
	{
		const CU::GrowingArray<WatchData>& collection = myWatchData[handleIt->second];

		for(int i = 0; i < collection.Size(); ++i)
		{
			const WatchData& data = collection[i];

			if (data.dirPath == aWatchData.dirPath && data.fileName == aWatchData.fileName)
			{
				return true;
			}
		}
	}

	return false;
}

void System::File::CFileWatcher::GetNewFiles()
{
	while (myAddFileQueue.IsEmpty() == false)
	{
		const WatchData addFile = myAddFileQueue.Pop();
		const bool inCollection = InCollection(addFile);

		if(inCollection == false)
		{
			CreateWatchData(addFile);
		}
	}
}

void System::File::CFileWatcher::NotifyFileChange(const WatchData& aWatchData) const
{
	//TODO: Notify the object that the file that it asked to watch was changed
}
