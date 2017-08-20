#pragma once
#include <commdlg.h>


namespace LeadowLib {

	enum DIALOG_FILE_TYPE
	{
		dft_file = 0x0001,
		dft_folder = 0x0002,
		dft_file_folder = dft_file | dft_folder
	};

	class CDlgGetFileName
	{
		friend UINT_PTR CALLBACK OpenDialogHookProc(
			_In_ HWND hdlg,
			_In_ UINT uiMsg,
			_In_ WPARAM wParam,
			_In_ LPARAM lParam
		);
	public:
		CDlgGetFileName();
		~CDlgGetFileName();
		BOOL OpenFile(HWND hOwner, DIALOG_FILE_TYPE type = dft_file);
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
		TCHAR* GetFileName(int index);

		static DWORD OPEN_FILE_OPTION;
		static DWORD SAVE_FILE_OPTION;
	private:
		OPENFILENAME m_ofn;
		CLdArray<CLdString*> m_Files;
		CLdMap<CLdString*, CLdString*> m_Filters;
		CLdString m_InitDir;
		CLdString m_ResultFiles;
		CLdString m_FilterTmp;
		CLdString m_DefaultExt;
		CLdString m_DefaultName;
		DWORD m_Option;

		TCHAR* GetFilterStr();
		OPENFILENAME* PrepareParam(HWND hOwner);
		VOID ProcessResult();
		BOOL VistaOpenFolder(HWND hOwner);
		BOOL XpOpenFolder(HWND hOwner);
	};

};