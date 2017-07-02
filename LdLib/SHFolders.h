#pragma once
#include <Shlobj.h>
#include "LdStructs.h"
#include "LdString.h"
#include "ldArray.h"
#include "LdMap.h"

typedef struct _SH_HEAD_INFO
{
	int fmt;
	int cxChar;
	TCHAR* szName;
}SH_HEAD_INFO, *PSH_HEAD_INFO;

class CSHFolders
{
public:
	//CSIDL_DESKTOP or CSIDL_BITBUCKET
	static HRESULT EnumFolderObjects(DWORD dwFolder, HWND hOwnerWnd, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param);
	//CSIDL_DESKTOP or CSIDL_BITBUCKET
	static HRESULT EnumFolderColumes(DWORD dwFolder, HWND hOwnerWnd, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param);
	//FOLDERID_Downloads
	static CLdString GetKnownFolderPath(const REFKNOWNFOLDERID rfid, DWORD dwFlag = KF_FLAG_DEFAULT, HANDLE hToken = NULL);

};

