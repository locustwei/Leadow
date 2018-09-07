#include "stdafx.h"
#include "NtfsUSN.h"


CNtfsUSN::CNtfsUSN(TCHAR* VolumePath):CMftChangeListener(VolumePath)
{
	m_Handle = INVALID_HANDLE_VALUE;
}


CNtfsUSN::~CNtfsUSN()
{
	if (m_Handle != INVALID_HANDLE_VALUE)
		CloseHandle(m_Handle);
}

BOOL CNtfsUSN::QueryUsnStatus(HANDLE hVolume, PUSN_JOURNAL_DATA outStatus)
{
	ULONG br;
	return DeviceIoControl(hVolume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, outStatus, sizeof(USN_JOURNAL_DATA), &br, NULL);
}

BOOL CNtfsUSN::CreateUsnJournal(HANDLE hVolume)
{
	USN_JOURNAL_DATA qujd = { 0 };

	if (QueryUsnStatus(hVolume, &qujd))
		return TRUE;

	if (GetLastError() == ERROR_JOURNAL_NOT_ACTIVE)
	{
		CREATE_USN_JOURNAL_DATA cujd = { 1024 * 1024 * 100, 1024 * 1024 * 20 };
		ULONG br;
		return DeviceIoControl(hVolume, FSCTL_CREATE_USN_JOURNAL, &cujd, sizeof(cujd), NULL, 0, &br, NULL);
	}
	else
		return TRUE;
}

BOOL CNtfsUSN::DeleteUsnJournal(HANDLE hVolume)
{
	USN_JOURNAL_DATA uujd = { 0 };
	if (!QueryUsnStatus(hVolume, &uujd))
		return FALSE;
	ULONG br;
	DELETE_USN_JOURNAL_DATA dujd = { uujd.UsnJournalID, USN_DELETE_FLAG_DELETE || USN_DELETE_FLAG_NOTIFY };
	return DeviceIoControl(hVolume, FSCTL_DELETE_USN_JOURNAL, &dujd, sizeof(dujd), NULL, 0, &br, NULL);
}

USN CNtfsUSN::UpdateFiles()
{

	if (m_Handle == INVALID_HANDLE_VALUE)
	{
		TCHAR* tmp = new TCHAR[_tcslen(m_Path) + 4 + 1];
		_tcscpy(tmp, _T("\\\\.\\"));
		_tcscat(tmp, m_Path);
		tmp[_tcslen(tmp) - 1] = '\0';

		m_Handle = CreateFile(tmp, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);

		delete[]tmp;

		if (m_Handle == INVALID_HANDLE_VALUE)
			return 0;
	}

	USN_JOURNAL_DATA qujd = { 0 };

	if (!QueryUsnStatus(m_Handle, &qujd))
		return 0;

	if (m_USN >= qujd.NextUsn)
		return m_USN;

	USN ret = m_USN > qujd.FirstUsn ? m_USN : qujd.FirstUsn;

	MFT_FILE_DATA tmpData;

	USN_CHANGED_REASON reason;

	PUCHAR buffer = new UCHAR[USN_PAGE_SIZE];

	READ_USN_JOURNAL_DATA rujd = { ret, USN_REASON_CLOSE, 0, 0, 0, qujd.UsnJournalID };
		/*USN_REASON_FILE_CREATE | USN_REASON_FILE_DELETE | USN_REASON_FILE_CREATE | USN_REASON_RENAME_OLD_NAME0xffffffff/*to get all reason*/
		
	ULONG cb = 0;
	BOOL fOk = TRUE;
	while (fOk) 
	{
		fOk = DeviceIoControl(m_Handle, FSCTL_READ_USN_JOURNAL, &rujd, sizeof(rujd), buffer, USN_PAGE_SIZE, &cb, NULL);
		if (!fOk || (cb <= sizeof(USN)))
		{
			break;
		}
		rujd.StartUsn = *(USN*)buffer;

		USN_RECORD *pUsnRecord = (PUSN_RECORD)&buffer[sizeof(USN)];

		while (true) 
		{

			if (pUsnRecord->Reason & USN_REASON_CLOSE)
			{
				ZeroMemory(&tmpData, sizeof(MFT_FILE_DATA));
					
				if (pUsnRecord->Reason & USN_REASON_FILE_DELETE)      //删除文件
					reason = USN_FILE_DELETE;
				else if (pUsnRecord->Reason & USN_REASON_FILE_CREATE) // || record->Reason & USN_REASON_RENAME_NEW_NAME){ //新文件
				{
					reason = USN_FILE_CREATE;
					tmpData.FileAttributes = pUsnRecord->FileAttributes;
					tmpData.DirectoryFileReferenceNumber = pUsnRecord->ParentFileReferenceNumber & 0x0000FFFFFFFFFFFF;
					tmpData.NameLength = pUsnRecord->FileNameLength;
					CopyMemory(tmpData.Name, (PUCHAR)pUsnRecord + pUsnRecord->FileNameOffset, tmpData.NameLength * sizeof(WCHAR));
					tmpData.Name[tmpData.NameLength] = '\0';
				}
				else if ((pUsnRecord->Reason & USN_REASON_RENAME_NEW_NAME) && ((pUsnRecord->Reason & USN_REASON_RENAME_OLD_NAME) == 0))//更名
				{
					reason = USN_RENAME_NEW_NAME;
					tmpData.FileAttributes = pUsnRecord->FileAttributes;
					tmpData.DirectoryFileReferenceNumber = pUsnRecord->ParentFileReferenceNumber & 0x0000FFFFFFFFFFFF;
					tmpData.NameLength = pUsnRecord->FileNameLength;
					CopyMemory(tmpData.Name, (PUCHAR)pUsnRecord + pUsnRecord->FileNameOffset, tmpData.NameLength * sizeof(WCHAR));
					tmpData.Name[tmpData.NameLength] = '\0';
				}
				else if (pUsnRecord->Reason & (USN_REASON_DATA_OVERWRITE | USN_REASON_DATA_TRUNCATION | USN_REASON_DATA_EXTEND))
					reason = USN_DATA_OVERWRITE;
				else
					reason = USN_FILE_UNKONW;

				tmpData.FileReferenceNumber = pUsnRecord->FileReferenceNumber & 0x0000FFFFFFFFFFFF;

				fOk = m_Handler->OnFileChangedCallback(reason, &tmpData, m_Param);
				if (!fOk)
					break;
			}

			pUsnRecord = (PUSN_RECORD)((PBYTE)pUsnRecord + pUsnRecord->RecordLength);
			if(pUsnRecord->RecordLength == 0 || cb <= pUsnRecord->RecordLength )
				break;
			cb -= pUsnRecord->RecordLength;
		}
			
		ret = rujd.StartUsn;
	}

	if (buffer)
		delete[]buffer;

	m_USN = ret;
	return ret;
}

USN CNtfsUSN::ListenUpdate(HANDLE hStopEvent, IMftChangeListenerHandler * handler, UINT_PTR param)
{
	__super::ListenUpdate(hStopEvent, handler, param);

	m_USN = UpdateFiles();

	/*HANDLE hMonitor = FindFirstChangeNotification(m_Path, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE);

	HANDLE waitfor[2];
	waitfor[0] = hStopEvent;
	waitfor[1] = hMonitor;
*/
	do
	{

		//if (hMonitor != INVALID_HANDLE_VALUE)
		{
			DWORD n = WaitForSingleObject(hStopEvent, 10 * 1000);
			/*if (n == WAIT_OBJECT_0)
				break;
			else */
			if (n == WAIT_TIMEOUT)
			{
				//Sleep(1000);
				m_USN = UpdateFiles();
				/*if (FALSE == FindNextChangeNotification(hMonitor))
					break;*/
			}else //if(n != WAIT_TIMEOUT)
				break;

			if (handler->Stop(param))
				break;
		}

	} while (TRUE);

	/*if (hMonitor != INVALID_HANDLE_VALUE)
		FindCloseChangeNotification(hMonitor);*/

	return m_USN;
}