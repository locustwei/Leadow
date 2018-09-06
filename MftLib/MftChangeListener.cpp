#include "stdafx.h"
#include "MftChangeListener.h"
#include "NtfsUSN.h"


CMftChangeListener::CMftChangeListener(TCHAR* VolumePath)
{
	m_USN = 0;
	m_Path = VolumePath;
}


CMftChangeListener::~CMftChangeListener()
{
}

USN CMftChangeListener::ListenUpdate(HANDLE hStopEvent, IMftChangeListenerHandler* handler, UINT_PTR param)
{
	m_Handler = handler;
	m_Param = param;
	return 0;
}

CMftChangeListener* CMftChangeListener::CreateListener(TCHAR* VolumePath, UINT fs)
{
	if (VolumePath == nullptr)
	{
		return nullptr;
	}

	CMftChangeListener* Listener = nullptr;

	switch (fs)
	{
	case FILESYSTEM_STATISTICS_TYPE_NTFS:
		Listener = new CNtfsUSN(VolumePath);
		break;
	case FILESYSTEM_STATISTICS_TYPE_FAT:
		//Listener = new CFatMftReader(hVolume);
		break;
	case FILESYSTEM_STATISTICS_TYPE_EXFAT:
		//Listener = new CExFatReader(hVolume);
		break;
	default:
		return nullptr;
	}

	return Listener;
}

USN CMftChangeListener::GetLastUsn()
{
	return m_USN;
}

VOID CMftChangeListener::SetLastUsn(USN usn)
{
	m_USN = usn;
}
