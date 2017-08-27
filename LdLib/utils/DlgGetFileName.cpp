#include "stdafx.h"
#include "../classes/LdString.h"
#include "../classes/LdMap.h"
#include "DlgGetFileName.h"
#include <shlobj.h>
#include "../LdLib.h"

#pragma comment (lib, "Shlwapi.lib")

#define OFN_FOLDER_FILES (0x20000000 | OFN_ENABLEHOOK)      //同时选择文件和文件夹
#define RESULT_BUFFER_LEN (1024 * 1024)

namespace LeadowLib {
	DWORD CDlgGetFileName::OPEN_FILE_OPTION = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ENABLESIZING;
	DWORD CDlgGetFileName::SAVE_FILE_OPTION = OFN_PATHMUSTEXIST | OFN_ENABLESIZING;

/*
	class CFileDialogEventHandler :
		public IFileDialogEvents,
		public IFileDialogControlEvents
	{
	public:

		static HRESULT CreateInstance(REFIID riid, void **ppv)
		{
			*ppv = NULL;
			CFileDialogEventHandler* pFileDialogEventHandler = new CFileDialogEventHandler();
			HRESULT hr = pFileDialogEventHandler ? S_OK : E_OUTOFMEMORY;
			if (SUCCEEDED(hr))
			{
				hr = pFileDialogEventHandler->QueryInterface(riid, ppv);
				pFileDialogEventHandler->Release();
			}
			return hr;
		}

	protected:

		IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
		{
			static const QITAB qit[] =
			{
				QITABENT(CFileDialogEventHandler, IFileDialogEvents),
				QITABENT(CFileDialogEventHandler, IFileDialogControlEvents),
				{ nullptr }
			};
			return QISearch(this, qit, riid, ppv);
		}

		IFACEMETHODIMP_(ULONG) AddRef()
		{
			return InterlockedIncrement(&m_cRef);
		}

		IFACEMETHODIMP_(ULONG) Release()
		{
			long cRef = InterlockedDecrement(&m_cRef);
			if (!cRef)
			{
				delete this;
			}
			return cRef;
		}

		// 
		// IFileDialogEvents methods
		// 

		IFACEMETHODIMP OnFileOk(IFileDialog* pfd)
		{
			DebugOutput(L"OnFileOk\n");
			IFileOpenDialog* dlg;
			IShellItem* item;
			pfd->GetCurrentSelection(&item);
			
			pfd->QueryInterface(IID_IFileOpenDialog, (void**)&dlg);
			IShellItemArray* items;
			dlg->GetSelectedItems(&items);
			DWORD nCount;
			items->GetCount(&nCount);
			for (DWORD i = 0; i < nCount; i++)
			{
				IShellItem* item;
				items->GetItemAt(i, &item);
				TCHAR* s;
				item->GetDisplayName(SIGDN_FILESYSPATH, &s);
				OutputDebugString(s);
				OutputDebugStringA("\n");
			}
			return S_OK;
		}
		IFACEMETHODIMP OnFolderChange(IFileDialog*)
		{
			DebugOutput(L"OnFolderChange\n");
			return E_NOTIMPL;
		}
		IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*)
		{
			DebugOutput(L"OnFolderChanging\n");
			return E_NOTIMPL;
		}
		IFACEMETHODIMP OnHelp(IFileDialog*)
		{
			DebugOutput(L"OnHelp\n");
			return E_NOTIMPL;
		}
		IFACEMETHODIMP OnSelectionChange(IFileDialog* pfd)
		{
			IShellItem* item;
			
			if (pfd->GetResult(&item) == S_OK && item)
			{
				TCHAR* s;
				item->GetDisplayName(SIGDN_FILESYSPATH, &s);
				DebugOutput(L"OnSelectionChange %s\n", s);
			}
			return E_NOTIMPL;
		}
		IFACEMETHODIMP OnTypeChange(IFileDialog*)
		{
			DebugOutput(L"OnTypeChange\n");
			return E_NOTIMPL;
		}
		IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*)
		{
			DebugOutput(L"OnShareViolation\n");
			return E_NOTIMPL;
		}
		IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*)
		{
			DebugOutput(L"OnOverwrite\n");
			return E_NOTIMPL;
		}

		// 
		// IFileDialogControlEvents methods
		// 

		IFACEMETHODIMP OnItemSelected(IFileDialogCustomize*pfdc, DWORD dwIDCtl, DWORD dwIDItem)
		{
			DebugOutput(L"OnItemSelected %d %d\n", dwIDCtl, dwIDItem);
			return E_NOTIMPL;
		}

		IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize*, DWORD dwID)
		{
			DebugOutput(L"OnButtonClicked\n");
			return E_NOTIMPL;
		}
		IFACEMETHODIMP OnControlActivating(IFileDialogCustomize*, DWORD dwID)
		{
			DebugOutput(L"OnControlActivating\n");
			return E_NOTIMPL;
		}
		IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize*, DWORD dwID, BOOL)
		{
			DebugOutput(L"OnCheckButtonToggled\n");
			return E_NOTIMPL;
		}

		CFileDialogEventHandler() : m_cRef(1) { }

	private:

		~CFileDialogEventHandler() { }
		long m_cRef;
	};

*/

//UINT _afxMsgLBSELCHANGE = 0;
//UINT _afxMsgSHAREVI = 0;
//UINT _afxMsgFILEOK = 0;
//UINT _afxMsgCOLOROK = 0;
//UINT _afxMsgHELP = 0;
//UINT _afxMsgSETRGB = 0;

	UINT_PTR CALLBACK OpenDialogHookProc(
		_In_ HWND hdlg,
		_In_ UINT uiMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam
	)
	{
		HWND hPerant;
		WNDPROC proc;

		switch (uiMsg)
		{
		case WM_INITDIALOG:
			SetProp(hdlg, _T("obj"), (HANDLE)lParam);
			//_afxMsgLBSELCHANGE = ::RegisterWindowMessage(LBSELCHSTRING);
			//_afxMsgFILEOK = ::RegisterWindowMessage(FILEOKSTRING);
			hPerant = GetParent(hdlg);
			if (hPerant) //上层窗口才是真正的Dialog
			{
				LONG_PTR old = SetWindowLongPtr(hPerant, GWLP_WNDPROC, (LONG_PTR)OpenDialogHookProc);
				if (old)
				{
					SetProp(hPerant, _T("obj"), (HANDLE)lParam);
					SetProp(hPerant, _T("old"), (HANDLE)old);
				}
			}
			return true;
		case WM_COMMAND:
			if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDOK)
			{
				CDlgGetFileName* This = (CDlgGetFileName*)GetProp(hdlg, _T("obj"));
				if (!This)
					return 0;

				HWND hParent = hdlg;// GetParent(hdlg);
				if (!hParent)
					return 0;
				HWND hList = ::GetDlgItem(hParent, lst2);
				if (!hList)
					return 0;
				hList = GetDlgItem(hList, 1);
				if (!hList)
					return 0;

				int i = -1, count = ListView_GetSelectedCount(hList);
				if (count <= 0)
					return 0;

				TCHAR path[MAX_PATH] = { 0 };
				if (SendMessage(hParent, CDM_GETFOLDERPATH, (WPARAM)MAX_PATH, (LPARAM)path) < 0)
					return 0;

				This->ClearFiles();
				if (path[_tcslen(path) - 1] != '\\')
					path[_tcslen(path)] = '\\';
				TCHAR fileName[MAX_PATH] = { 0 };
				while (count-- > 0)
				{
					i = ListView_GetNextItem(hList, i, LVNI_SELECTED);
					if (i < 0)
						break;
					ListView_GetItemText(hList, i, 0, fileName, MAX_PATH);
					CLdString* s = new CLdString(path);
					s->Append(fileName);
					This->m_Files.Add(s);
				}
				EndDialog(hdlg, IDOK);
				return 1;
			}
		case WM_DESTROY:
			break;
		}

		proc = (WNDPROC)GetProp(hdlg, _T("old"));
		if (!proc)
			return(0);
		else
			return CallWindowProc(proc, hdlg, uiMsg, wParam, lParam);
	}

	CDlgGetFileName::CDlgGetFileName() :
		m_Files(),
		m_Filters(),
		m_InitDir(),
		m_ofn(),
		m_ResultFiles((UINT)RESULT_BUFFER_LEN),
		m_FilterTmp((UINT)MAX_PATH),
		m_DefaultName(),
		m_Option(OPEN_FILE_OPTION)
	{
	}

	void CDlgGetFileName::ClearFiles()
	{
		for (int i = 0; i < m_Files.GetCount(); i++)
			m_Files.Delete(i);
		m_Files.Clear();
	}


	CDlgGetFileName::~CDlgGetFileName()
	{
		ClearFiles();
		for(int i=0; i<m_Filters.GetCount(); i++)
		{
			CLdString** value;
			CLdString** key = m_Filters.GetAt(i, &value);
			delete *value;
			delete *key;
		}
	}

	BOOL CDlgGetFileName::VistaOpenFolder(HWND hOwner)
	{
		HRESULT hr;

		IFileOpenDialog *pfd = NULL;
		COMDLG_FILTERSPEC* filters = nullptr;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if (SUCCEEDED(hr))
		{
			pfd->SetOptions( (m_Option | FOS_PICKFOLDERS) & (~OFN_EXPLORER) & (~OFN_ENABLESIZING)); 
			if (m_Filters.GetCount() > 0)
			{
				filters = new COMDLG_FILTERSPEC[m_Filters.GetCount()];
				for (int i = 0; i < m_Filters.GetCount(); i++)
				{
					CLdString** pName, **pSpec;
					pName = m_Filters.GetAt(i, &pSpec);
					filters[i].pszName = (*pName)->GetData();
					filters[i].pszSpec = (*pSpec)->GetData();
				}
				pfd->SetFileTypes(m_Filters.GetCount(), filters);
			}
			hr = pfd->Show(hOwner);
			if (SUCCEEDED(hr))
			{
				IShellItemArray* items;
				hr = pfd->GetResults(&items);
				if (SUCCEEDED(hr))
				{
					DWORD nCount;
					hr = items->GetCount(&nCount);
					for (DWORD i = 0; i < nCount; i++)
					{
						IShellItem* item;
						items->GetItemAt(i, &item);
						TCHAR* s;
						hr = item->GetDisplayName(SIGDN_FILESYSPATH, &s);
						if (SUCCEEDED(hr))
						{
							m_Files.Add(new CLdString(s));
							CoTaskMemFree(s);
						}
					}
				}
			}
			pfd->Release();
		}
		if (filters)
			delete filters;

		return SUCCEEDED(hr);
	}

	BOOL CDlgGetFileName::XpOpenFolder(HWND hOwner)
	{
		BOOL result = false;
		LPITEMIDLIST pidl = nullptr;
		BROWSEINFO info = { 0 };
		info.hwndOwner = hOwner;
		info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_VALIDATE;
		if (!m_InitDir.IsEmpty())
		{
			if (SHParseDisplayName(m_InitDir.GetData(), nullptr, &pidl, 0, nullptr) == S_OK)
				info.pidlRoot = pidl;
		}
		PIDLIST_ABSOLUTE pret = SHBrowseForFolder(&info);
		if (pret)
		{
			CLdString* s = new CLdString((UINT)MAX_PATH);
			if (SUCCEEDED(SHGetPathFromIDList(pret, s->GetData())))
			{
				m_Files.Add(s);
				result = true;
			}
			else
				delete s;
			CoTaskMemFree(pret);
		}
		return result;
	}

	BOOL CDlgGetFileName::OpenFile(HWND hOwner, DIALOG_FILE_TYPE type)
	{
		BOOL Result;

		if(type == dft_folder)
		{//选择文件夹
			if (GetOsType() < Windows_Vista)
			{//XP系统下用SHBrowseForFolder
				Result = XpOpenFolder(hOwner);
				
			}else
			{
				m_Option |= FOS_PICKFOLDERS;
				Result = VistaOpenFolder(hOwner);
			}
		}
		else
		{
			if (type == (dft_file_folder)) //文件和文件夹混合选择
				m_Option |= OFN_FOLDER_FILES;
			m_Option |= OFN_EXPLORER;
			PrepareParam(hOwner);
			Result = GetOpenFileName(&m_ofn);
			if (Result)
			{
				ProcessResult();
			}
		}
		return Result;
	}

	BOOL CDlgGetFileName::SaveFile(HWND hOwner)
	{
		PrepareParam(hOwner);

		BOOL result = GetSaveFileName(&m_ofn);

		if (result)
			ProcessResult();
		return result;
	}

	VOID CDlgGetFileName::SetDefaultExt(TCHAR* lpExt)
	{
		m_DefaultExt = lpExt;
	}

	VOID CDlgGetFileName::SetDefaultName(TCHAR* lpFileName)
	{
		m_DefaultName = lpFileName;
	}

	VOID CDlgGetFileName::AddFilter(TCHAR* pszName, TCHAR* pszSpec)
	{
		m_Filters.Put(new CLdString(pszName), new CLdString(pszSpec));
	}

	VOID CDlgGetFileName::SetOption(DWORD dwOption)
	{
		m_Option = dwOption;
	}

	VOID CDlgGetFileName::SetInitDir(TCHAR* lpFolder)
	{
		m_InitDir = lpFolder;
	}

	int CDlgGetFileName::GetFileCount()
	{
		return m_Files.GetCount();
	}

	TCHAR* CDlgGetFileName::GetFileName(int index)
	{
		if (index > GetFileCount() - 1)
			return nullptr;
		 return m_Files[index]->GetData();
	}

	TCHAR* CDlgGetFileName::GetFilterStr()
	{
		m_FilterTmp.SetLength(1024);
		TCHAR* p = m_FilterTmp.GetData();

		for (int i = 0; i<m_Filters.GetCount(); i++)
		{
			CLdString** value;
			CLdString** key = m_Filters.GetAt(i, &value);
			(*key)->CopyTo(p);
			p += (*key)->GetLength() + 1;
			(*value)->CopyTo(p);
			p += (*value)->GetLength() + 1;
		}
		return m_FilterTmp.GetData();
	}

	OPENFILENAME * CDlgGetFileName::PrepareParam(HWND hOwner)
	{
		ZeroMemory(&m_ofn, sizeof(m_ofn));
		m_ofn.lStructSize = sizeof(m_ofn);
		m_ofn.hwndOwner = hOwner;
		m_ofn.lpstrFile = m_ResultFiles;
		if (!m_DefaultName.IsEmpty())
			m_DefaultName.CopyTo(m_ofn.lpstrFile);
		else
			m_ofn.lpstrFile[0] = '\0';
		m_ofn.lpstrFilter = GetFilterStr();// _T("All\0*.*\0Text\0*.TXT\0");
		m_ofn.nFilterIndex = 1;
		m_ofn.lpstrFileTitle = NULL;
		m_ofn.nMaxFileTitle = 0;
		m_ofn.nMaxFile = RESULT_BUFFER_LEN;
		m_ofn.lpstrInitialDir = m_InitDir;

		m_ofn.Flags = m_Option;
		m_ofn.lpstrDefExt = m_DefaultExt;
		m_ofn.lpfnHook = OpenDialogHookProc;
		m_ofn.lCustData = (LPARAM)this; //system sends the WM_INITDIALOG message to the hook procedure
		return &m_ofn;
	}

	VOID CDlgGetFileName::ProcessResult()
	{
		if ((m_Option & OFN_FOLDER_FILES) == (OFN_FOLDER_FILES) && m_Files.GetCount() != 0)
			return; //done OpenDialogHookProc

		TCHAR* p = m_ResultFiles;

		size_t k = 0;
		while (p[0] != '\0' && k < RESULT_BUFFER_LEN)
		{
			k += _tcslen(p) + 1;
			p = m_ResultFiles.GetData() + k;
			if (p[0] != '\0')
			{
				CLdString* s = new CLdString(m_ResultFiles.GetData());
				s->Append(_T("\\"));
				s->Append(p);
				m_Files.Add(s);
			}
			else if (m_Files.GetCount() == 0)  //只选了一个文件
				m_Files.Add(new CLdString(m_ResultFiles));  
		}
	}
}