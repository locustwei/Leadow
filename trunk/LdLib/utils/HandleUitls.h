#pragma once

#include "../ldapp/PublicRoutimes.h"

namespace LeadowLib {
	typedef enum _SYSTEM_HANDLE_TYPE
	{
		OB_TYPE_UNKNOWN = 0,    //0
		OB_TYPE_TYPE,           // 1,fixed
		OB_TYPE_DIRECTORY,      // 2,fixed
		OB_TYPE_SYMBOLIC_LINK,  // 3,fixed
		OB_TYPE_TOKEN,          // 4,fixed
		OB_TYPE_PROCESS,        // 5,fixed
		OB_TYPE_THREAD,         // 6,fixed
		OB_TYPE_JOB,            // 7,fixed
		OB_TYPE_DEBUG_OBJECT,   // 8,fixed
		OB_TYPE_EVENT,          // 9,fixed
		OB_TYPE_EVENT_PAIR,     //10,fixed
		OB_TYPE_MUTANT,         //11,fixed
		OB_TYPE_CALLBACK,       //12,fixed
		OB_TYPE_SEMAPHORE,      //13,fixed
		OB_TYPE_TIMER,          //14,fixed
		OB_TYPE_PROFILE,        //15,fixed
		OB_TYPE_KEYED_EVENT,    //16,fixed
		OB_TYPE_WINDOWS_STATION,//17,fixed
		OB_TYPE_DESKTOP,        //18,fixed
		OB_TYPE_SECTION,        //19,fixed
		OB_TYPE_KEY,            //20,fixed
		OB_TYPE_PORT,           //21,fixed 
		OB_TYPE_WAITABLE_PORT,  //22,fixed
		OB_TYPE_ADAPTER,        //23,fixed
		OB_TYPE_CONTROLLER,     //24,fixed
		OB_TYPE_DEVICE,         //25,fixed
		OB_TYPE_DRIVER,         //26,fixed
		OB_TYPE_IOCOMPLETION,   //27,fixed
		OB_TYPE_FILE,           //28,fixed
		OB_TYPE_WMIGUID,        //29,fixed
		OB_TYPE_SESTION,        //30
		OB_TYPE_WORKER_FACTORY, //31
		OB_TYPE_USER_APC_RESERVE//32
	}SYSTEM_HANDLE_TYPE;

	class IEnumSystemHandleCallback
	{
	public:
		virtual BOOL SystemHandleCallback(PSYSTEM_HANDLE pHandle, PVOID pParam) = 0;
	};

	class CHandleUitls
	{
	public:
		static DWORD EnumSystemHandleInfo(IEnumSystemHandleCallback* callback, PVOID pParam);
		static DWORD FindOpenFileHandle(LPTSTR lpFullFileName, IEnumSystemHandleCallback* callback, PVOID pParam);
		static DWORD DuplicateSysHandle(PSYSTEM_HANDLE pHandle, HANDLE& hOutHandle);
		static DWORD GetSysHandleName(HANDLE hDupHandle, LPTSTR lpName);
		static DWORD GetSysHandleType(HANDLE hDupHandle, SYSTEM_HANDLE_TYPE& sysType);
	private:
	};

}