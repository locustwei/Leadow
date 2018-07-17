#include "stdafx.h"
#include "MftChangeListener.h"


CMftChangeListener::CMftChangeListener(HANDLE hVoluem)
{
	m_USN = 0;
	m_Handle = hVoluem;
}


CMftChangeListener::~CMftChangeListener()
{
}

USN CMftChangeListener::ListenUpdate(IMftChangeListenerHandler* handler, PVOID param)
{
	m_Handler = handler;
	m_Param = param;

	//HANDLE hMonitor = INVALID_HANDLE_VALUE;
	//if (!m_VolumePath.IsEmpty()) {
	//	CLdString path = m_VolumePath;
	//	path += '\\';
	//	hMonitor = FindFirstChangeNotification(path, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE);
	//}
	do 
	{
		m_USN = UpdateFiles();
		if (handler->ListenerStop())
			break;

		//if (hMonitor != INVALID_HANDLE_VALUE)
		//{
		//	WaitForSingleObject(hMonitor, INFINITE);
		//	if (FALSE == FindNextChangeNotification(hMonitor))
		//		break;
		//}
		Sleep(10 * 1000);
	} while (TRUE);

	//if (hMonitor != INVALID_HANDLE_VALUE)
	//	FindCloseChangeNotification(hMonitor);

	return 0;
}

USN CMftChangeListener::GetLastUsn()
{
	return m_USN;
}

VOID CMftChangeListener::SetLastUsn(USN usn)
{
	m_USN = usn;
}
