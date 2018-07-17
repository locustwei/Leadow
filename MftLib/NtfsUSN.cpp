#include "stdafx.h"
#include "NtfsUSN.h"


CNtfsUSN::CNtfsUSN(HANDLE hVoluem):CMftChangeListener(hVoluem)
{
}


CNtfsUSN::~CNtfsUSN()
{
}

BOOL CNtfsUSN::QueryUsnStatus(HANDLE hVolume, PUSN_JOURNAL_DATA outStatus)
{
	ULONG br;
	return DeviceIoControl(hVolume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, outStatus, sizeof(USN_JOURNAL_DATA), &br, NULL);
}

BOOL CNtfsUSN::CreateUsnJournal(HANDLE hVolume)
{
	CREATE_USN_JOURNAL_DATA cujd = { 1024 * 1024 * 100, 1024 * 1024 * 20 };
	ULONG br;
	return DeviceIoControl(hVolume, FSCTL_CREATE_USN_JOURNAL, &cujd, sizeof(cujd), NULL, 0, &br, NULL);
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
		return 0;

	USN_JOURNAL_DATA qujd = { 0 };

	if (!QueryUsnStatus(m_Handle, &qujd))
		return 0;

	USN ret = 0;

	USN_CHANGED_REASON reason;

	if (m_USN >= qujd.FirstUsn && m_USN < qujd.NextUsn) {
		BYTE* buffer = new BYTE[USN_PAGE_SIZE];
		if (buffer) {
			ULONG BytesReturned = 0;
			READ_USN_JOURNAL_DATA rujd = { m_USN == 0 ? qujd.FirstUsn : m_USN ,
				USN_REASON_CLOSE/*USN_REASON_FILE_CREATE | USN_REASON_FILE_DELETE | USN_REASON_FILE_CREATE | USN_REASON_RENAME_OLD_NAME0xffffffff/*to get all reason*/,
				0, 0, 0, qujd.UsnJournalID };
			memset(buffer, 0, USN_PAGE_SIZE);
			ULONG cb = 0;
			BOOL fOk = TRUE;
			while (fOk) {
				fOk = DeviceIoControl(m_Handle, FSCTL_READ_USN_JOURNAL, &rujd, sizeof(rujd), buffer, USN_PAGE_SIZE, &cb, NULL);
				if (!fOk || (cb <= sizeof(USN)))
					break;

				rujd.StartUsn = *(USN*)buffer;

				USN_RECORD *pUsnRecord = (PUSN_RECORD)&buffer[sizeof(USN)];

				while ((PBYTE)pUsnRecord < (buffer + cb)) 
				{
					if (pUsnRecord->Reason & USN_REASON_CLOSE)
					{
						if (pUsnRecord->Reason & USN_REASON_FILE_DELETE)      //删除文件
							reason = USN_FILE_DELETE;
						else if (pUsnRecord->Reason & USN_REASON_FILE_CREATE) // || record->Reason & USN_REASON_RENAME_NEW_NAME){ //新文件
						{
							reason = USN_FILE_CREATE;
						}
						else if (pUsnRecord->Reason & USN_REASON_RENAME_NEW_NAME) //更名
						{
							reason = USN_RENAME_NEW_NAME;
						}
						else if (pUsnRecord->Reason & (USN_REASON_DATA_OVERWRITE | USN_REASON_DATA_TRUNCATION | USN_REASON_DATA_EXTEND))
							reason = USN_DATA_OVERWRITE;
						else
							reason = USN_FILE_UNKONW;

						fOk = m_Handler->OnFileChangedCallback(reason, pUsnRecord->FileReferenceNumber & 0x0000FFFFFFFFFFFF, m_Param);
						if (!fOk)
							break;
					}

					pUsnRecord = (PUSN_RECORD)
						((PBYTE)pUsnRecord + pUsnRecord->RecordLength);
				}
			}
			if (::GetLastError() == ERROR_JOURNAL_ENTRY_DELETED) {
				ret = 0;
			}
		}
		if (buffer) {
			delete[] buffer;
		}

		ret = qujd.NextUsn;
	}
	m_USN = ret;
	return ret;
}