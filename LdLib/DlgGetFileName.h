#pragma once


class CDlgGetFileName
{
public:
	CDlgGetFileName();
	~CDlgGetFileName();

	BOOL OpenFile(HWND hOwner);
	BOOL SaveFile(HWND hOwner);
	VOID SetDefaultExt(LPCTSTR lpExt);
	VOID AddFilter(LPCTSTR lpFilter);
	VOID SetOption(DWORD dwOption);
	VOID SetInitDir(LPCTSTR lpFolder);

	static DWORD OPEN_FILE_OPTION;
	static DWORD SAVE_FILE_OPTION;
private:
	CLdList<CLdString> m_Files;
	CLdList<LPCTSTR> m_Filters;
	CLdString m_InitDir;
	DWORD m_Option;
	LPCWSTR GetFilterStr();
};

