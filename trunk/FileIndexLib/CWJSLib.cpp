#include "CWJSLib.h"


CWJSLib::CWJSLib(void):
	m_Volumes()
{
	EnumDiskVolumes();
}


CWJSLib::~CWJSLib(void)
{
	for(ULONG i=0; i<m_Volumes.size(); i++)
		delete m_Volumes[i];
}


VOID CWJSLib::EnumDiskVolumes()
{
	WCHAR Volume[MAX_PATH] = {0};
	ULONG CharCount = MAX_PATH;

	HANDLE hFind = FindFirstVolume(Volume, ARRAYSIZE(Volume));
	if(hFind == INVALID_HANDLE_VALUE)
		return ;

	while(TRUE){

		size_t Index = wcslen(Volume) - 1;
		if(Index < 0)
			Index = 0;

		if (Volume[0]      != L'\\' ||
			Volume[1]      != L'\\' ||
			Volume[2]      != L'?'  ||
			Volume[3]      != L'\\' ||
			Volume[Index] != L'\\'){

#ifdef _DEBUG
				wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n", Volume);
#endif // _DEBUG
				
				break;
		}

		CDiskVolume* volume = new CDiskVolume();
		if(!volume->OpenVolume(Volume)){
			delete volume;
		}else{
			m_Volumes.Add(volume);
		}

		ZeroMemory(Volume, ARRAYSIZE(Volume));

		if(!FindNextVolume(hFind, Volume, ARRAYSIZE(Volume))){
			break;
		}
	}

	FindVolumeClose(hFind);
}

BOOL CWJSLib::SetDumpFilePath(PCWSTR wsz_path)
{
	for (std::CLdArray<CDiskVolume*>::iterator it = m_Volumes.begin() ; it != m_Volumes.end(); ++it)
		(*it)->SetDumpFilePath(wsz_path);
	return TRUE;
}

UINT  CWJSLib::GetVolumeCount()
{
	return (UINT)m_Volumes.size();
}


IVolumeInterface* CWJSLib::GetVolume(ULONG idx)
{
	if (idx >= m_Volumes.size())
		return NULL;
	else
		return m_Volumes[idx];
}
