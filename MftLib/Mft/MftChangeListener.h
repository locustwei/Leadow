#pragma once
#include "MftReader.h"

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
	virtual BOOL OnFileChangedCallback(USN_CHANGED_REASON, PMFT_FILE_DATA, UINT_PTR) = 0;
	virtual BOOL Stop(UINT_PTR) = 0;
};

class CMftChangeListener
{
public:
	CMftChangeListener(TCHAR* VolumePath);
	virtual ~CMftChangeListener();

	USN GetLastUsn();
	VOID SetLastUsn(USN usn);
	virtual USN ListenUpdate(HANDLE hStopEvent, IMftChangeListenerHandler*, UINT_PTR param);

	static CMftChangeListener* CreateListener(TCHAR*, UINT fs);

protected:
	IMftChangeListenerHandler* m_Handler;
	TCHAR* m_Path;
	UINT_PTR m_Param;
	USN m_USN;
	virtual USN UpdateFiles() = 0;
};

