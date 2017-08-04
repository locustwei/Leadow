#pragma once
#include <commdlg.h>

namespace LeadowLib {
	class CDlgGetFileName
	{
	public:
		CDlgGetFileName();
		~CDlgGetFileName();
		BOOL OpenFile(HWND hOwner);
		BOOL SaveFile(HWND hOwner);
		VOID SetDefaultExt(TCHAR* lpExt);
		VOID SetDefaultName(TCHAR* lpFileName);
		//��ӹ����磺 "Word Documents",   "*.doc;*.docx"
		VOID AddFilter(TCHAR* pszName, TCHAR* pszSpec);
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
		CLdMap<CLdString, CLdString> m_Filters;
		CLdString m_InitDir;
		CLdString m_ResultFiles;
		CLdString m_FilterTmp;
		CLdString m_DefaultExt;
		CLdString m_DefaultName;
		DWORD m_Option;

		TCHAR* GetFilterStr();
		OPENFILENAME* PrepareParam(HWND hOwner);
		VOID ProcessResult();
		BOOL VistaOpenDialog(HWND hWnd);
	};

};