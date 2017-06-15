#include "stdafx.h"
#include "LdString.h"
#include "LdList.h"
#include "LdMap.h"
#include "DlgGetFileName.h"


DWORD CDlgGetFileName::OPEN_FILE_OPTION = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
DWORD CDlgGetFileName::SAVE_FILE_OPTION = OFN_EXPLORER | OFN_PATHMUSTEXIST;

UINT_PTR CALLBACK OFNHookProc(
	_In_ HWND hdlg,
	_In_ UINT uiMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	return 0;
}

CDlgGetFileName::CDlgGetFileName() :
	m_Files(),
	m_Filters(),
	m_InitDir()
{
	m_Option = 0;
}


CDlgGetFileName::~CDlgGetFileName()
{
}

BOOL CDlgGetFileName::OpenFile(HWND hOwner)
{
	BOOL Result = FALSE;

	OPENFILENAME ofn;       
	TCHAR szFile[260] = { 0 };
	HWND hwnd = NULL;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hOwner;
	ofn.lpstrFile = szFile;

	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = GetFilterStr();// _T("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = m_InitDir;
	ofn.Flags = m_Option;


	Result = GetOpenFileName(&ofn);
	if (ofn.lpstrFilter)
		delete[] ofn.lpstrFilter;
	return Result;

}

VOID CDlgGetFileName::AddFilter(LPCTSTR lpFilter)
{
	m_Filters.Append(lpFilter);
}

VOID CDlgGetFileName::SetOption(DWORD dwOption)
{
	m_Option = dwOption;
}

LPCTSTR CDlgGetFileName::GetFilterStr()
{
	TCHAR* result = new TCHAR[MAX_PATH];
	memset(result, 0, MAX_PATH * sizeof(TCHAR));
	int k = 0;
	TCHAR* p = result;
	for (int i = 0; i < m_Filters.GetCount(); i++)
	{
		if(value.GetLength() == 0)
			continue;
		key.CopyTo(p);
		p += key.GetLength();
		if (key.GetLength() > 0)
			p++;
		value.CopyTo(p);
		p += value.GetLength() + 1;
	}
	return result;
}