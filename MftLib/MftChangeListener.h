#pragma once

enum USN_CHANGED_REASON
{
	USN_FILE_UNKONW,
	USN_FILE_DELETE,
	USN_FILE_CREATE,
	USN_RENAME_NEW_NAME,
	USN_DATA_OVERWRITE
};

interface IMftChangeListenerHandler
{
	virtual BOOL OnFileChangedCallback(USN_CHANGED_REASON, UINT64, PVOID) = 0;
	virtual BOOL ListenerStop() = 0;
};

class CMftChangeListener
{
public:
	CMftChangeListener(HANDLE hVoluem);
	virtual ~CMftChangeListener();

	USN GetLastUsn();
	VOID SetLastUsn(USN usn);
	virtual USN ListenUpdate(IMftChangeListenerHandler*, PVOID param);
protected:
	IMftChangeListenerHandler* m_Handler;
	HANDLE m_Handle;
	PVOID m_Param;
	USN m_USN;
	virtual USN UpdateFiles() = 0;
};

