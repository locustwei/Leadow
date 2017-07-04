#pragma once

#include <commdlg.h>

class CDlgGetFileName
{
public:
	CDlgGetFileName();
	~CDlgGetFileName();

	BOOL OpenFile(HWND hOwner);
	BOOL SaveFile(HWND hOwner);
	VOID SetDefaultExt(TCHAR* lpExt);
	VOID SetDefaultName(TCHAR* lpFileName);
	VOID AddFilter(TCHAR* lpFilter);
	VOID SetOption(DWORD dwOption);
	VOID SetInitDir(TCHAR* lpFolder);
	int GetFileCount();
	//************************************
	// Qualifier: �Ի��򷵻ص��ļ�������ѡʱ��0��Ϊ·����
	// Parameter: int index
	// Parameter: BOOL bWithPath �����������ļ�����
	//************************************
	CLdString GetFileName(int index, BOOL bWithPath = TRUE);

	static DWORD OPEN_FILE_OPTION;
	static DWORD SAVE_FILE_OPTION;
private:
	OPENFILENAME m_ofn;
	CLdArray<CLdString> m_Files;
	CLdArray<CLdString> m_Filters;
	CLdString m_InitDir;
	CLdString m_ResultFiles;
	CLdString m_FilterTmp;
	CLdString m_DefaultExt;
	CLdString m_DefaultName;
	DWORD m_Option;

	TCHAR* GetFilterStr();
	OPENFILENAME* PrepareParam(HWND hOwner);
	VOID ProcessResult();
};

