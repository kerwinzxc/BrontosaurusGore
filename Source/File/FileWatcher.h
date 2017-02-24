#pragma once
#include <Windows.h>
#include "../CommonUtilities/GrowingArray.h"
#include <mutex>
#include "../Container/LocklessQueue.h"
#include <map>

namespace std
{
	class thread;
}

namespace System
{
	namespace File
	{
		class CFileWatcher
		{
		public:
			CFileWatcher();
			explicit CFileWatcher(unsigned int aBufferSize);
			~CFileWatcher();
			void Start();
			void Stop();

			void AddFile(const std::string& aFilePath);
			void AddFile(const std::wstring& aFilePath);

			void RemoveFile(const std::string& aFilePath);
			void RemoveFile(const std::wstring& aFilePath);


		protected:
			struct WatchData
			{
				HANDLE watchHandle = NULL;
				std::wstring dirPath;
				std::wstring fileName;
			};

			using WatchDataCollection = CU::GrowingArray<WatchData>;

			void HandleWriteChange(HANDLE aDirHandle, const WatchDataCollection& aWatchData) const;
			void WatchChange();
			void Run();
			void WaitForFile();
			WatchData ParseWatchdata(const std::wstring& aFilePath);
			void RemoveFiles();
			HANDLE GetDirHandle(const std::wstring& aDirPath);
			void CreateWatchData(WatchData aAddFile);
			bool InCollection(const WatchData& aAddFile);
			void GetNewFiles();
			void NotifyFileChange(const WatchData& aWatchData) const;

			bool myIsRunning;
			std::thread* myRunThread;


			std::mutex myFileWaitLock;
			std::condition_variable myWaitForFile;;

			Container::CLocklessQueue<WatchData> myAddFileQueue;
			Container::CLocklessQueue<std::wstring> myRemoveFileQueue;

			std::map<HANDLE, WatchDataCollection> myWatchData;
			std::map<std::wstring, HANDLE> myDirHandles;
		};
	}
}
