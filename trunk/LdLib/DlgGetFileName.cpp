#include "stdafx.h"
#include "LdString.h"
#include "LdMap.h"
#include "DlgGetFileName.h"
#include <shlobj.h>
#include <shlwapi.h>

#pragma comment (lib, "Shlwapi.lib")

DWORD CDlgGetFileName::OPEN_FILE_OPTION = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
DWORD CDlgGetFileName::SAVE_FILE_OPTION = OFN_EXPLORER | OFN_PATHMUSTEXIST;

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
			{ 0 }
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
		OutputDebugString(L"OnFileOk");
		IFileOpenDialog* dlg;
		pfd->QueryInterface(IID_IFileOpenDialog, (void**)&dlg);
		IShellItemArray* items;
		dlg->GetSelectedItems(&items);
		DWORD nCount;
		items->GetCount(&nCount);
		for (int i = 0; i<nCount; i++)
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
		return E_NOTIMPL;
	}
	IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*)
	{
		return E_NOTIMPL;
	}
	IFACEMETHODIMP OnHelp(IFileDialog*)
	{
		return E_NOTIMPL;
	}
	IFACEMETHODIMP OnSelectionChange(IFileDialog*)
	{
		return E_NOTIMPL;
	}
	IFACEMETHODIMP OnTypeChange(IFileDialog*)
	{
		return E_NOTIMPL;
	}
	IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*)
	{
		return E_NOTIMPL;
	}
	IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*)
	{
		return E_NOTIMPL;
	}

	// 
	// IFileDialogControlEvents methods
	// 

	IFACEMETHODIMP OnItemSelected(IFileDialogCustomize*pfdc, DWORD dwIDCtl, DWORD dwIDItem)
	{
		return E_NOTIMPL;
	}

	IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize*, DWORD dwID)
	{
		return E_NOTIMPL;
	}
	IFACEMETHODIMP OnControlActivating(IFileDialogCustomize*, DWORD dwID)
	{
		return E_NOTIMPL;
	}
	IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize*, DWORD dwID, BOOL)
	{
		return E_NOTIMPL;
	}

	CFileDialogEventHandler() : m_cRef(1) { }

private:

	~CFileDialogEventHandler() { }
	long m_cRef;
};

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
	m_InitDir(),
	m_ofn(),
	m_ResultFiles((UINT)MAX_PATH),
	m_FilterTmp((UINT)MAX_PATH),
	m_DefaultName()
{
	m_Option = 0;
}


CDlgGetFileName::~CDlgGetFileName()
{
}

BOOL CDlgGetFileName::VistaOpenDialog(HWND hOwner)
{
	HRESULT hr = S_OK;

	IFileDialog *pfd = NULL;
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		pfd->SetOptions(m_Option);
		if(m_Filters.GetCount()>0)
		{
			COMDLG_FILTERSPEC* filters = new COMDLG_FILTERSPEC[m_Filters.GetCount()];
			for (int i = 0; i<m_Filters.GetCount(); i++)
			{
				CLdString* pName, *pSpec;
				pName = m_Filters.GetAt(i, &pSpec);
				filters[i].pszName = pName->GetData();
				filters[i].pszSpec = pSpec->GetData();
			}
			pfd->SetFileTypes(m_Filters.GetCount(), filters);
		}
		// Create an event handling object, and hook it up to the dialog.
		IFileDialogEvents *pfde = NULL;
		hr = CFileDialogEventHandler::CreateInstance(IID_PPV_ARGS(&pfde));
		if (SUCCEEDED(hr))
		{
			// Hook up the event handler.
			DWORD dwCookie = 0;
			hr = pfd->Advise(pfde, &dwCookie);
			if (SUCCEEDED(hr))
			{
				// Show the open file dialog.
				if (SUCCEEDED(hr))
				{
					hr = pfd->Show(hOwner);
					if (SUCCEEDED(hr))
					{
						// You can add your own code here to handle the results...
					}
				}

				// Unhook the event handler
				pfd->Unadvise(dwCookie);
			}
			pfde->Release();
		}
		pfd->Release();
	}

	return SUCCEEDED(hr);
}


BOOL CDlgGetFileName::OpenFile(HWND hOwner)
{
	BOOL Result = FALSE;
	

//	Result = GetOpenFileName(PrepareParam(hOwner));
//
//	if (Result)
//	{
//		ProcessResult();
//	}
	return Result;

}

BOOL CDlgGetFileName::SaveFile(HWND hOwner)
{
	BOOL result = GetSaveFileName(PrepareParam(hOwner));
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
	m_Filters.Put(pszName, pszSpec);
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
	if (m_Files.GetCount() > 1)
		return m_Files.GetCount() - 1;  //第0个是路径
	else
		return m_Files.GetCount();
}

CLdString CDlgGetFileName::GetFileName(int index, BOOL bWithPath)
{
	if (index > GetFileCount() - 1)
		return _T("");
	if (GetFileCount() > 1)
	{ //多选
		if (!bWithPath)
			return m_Files[index];
		else
			return m_Files[0] + _T("\\") + m_Files[index + 1];
	}
	else
		return m_Files[0];
}

TCHAR* CDlgGetFileName::GetFilterStr()
{
//	memset(m_FilterTmp.GetData(), 0, MAX_PATH * sizeof(TCHAR));
//	int k = 0;
//	TCHAR* p = m_FilterTmp;
//
//	for (int i = 0; i < m_Filters.GetCount(); i++)
//	{
//		CLdString s = m_Filters[i];
//		if(s.GetLength() == 0)
//			continue;
//		s.CopyTo(p);
//		p += s.GetLength() + 1;
//		k += s.GetLength() + 1;
//	}
//
//	for (int i = 0; i < k; i++)
//	{
//		if (m_FilterTmp[i] == '|')
//			m_FilterTmp.SetAt(i, '\0');
//	}
//	return m_FilterTmp.GetData();
	return nullptr;
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
	m_ofn.nMaxFile = MAX_PATH;
	m_ofn.lpstrInitialDir = m_InitDir;
	m_ofn.Flags = m_Option;
	m_ofn.lpstrDefExt = m_DefaultExt;
	m_ofn.lCustData = (LPARAM)this; //system sends the WM_INITDIALOG message to the hook procedure
	return &m_ofn;
}

VOID CDlgGetFileName::ProcessResult()
{
	TCHAR* p = m_ResultFiles;
	size_t k = 0;
	while (p[0] != '\0' && k < MAX_PATH)
	{
		m_Files.Add(p);
		k += _tcslen(p) + 1;
		p = m_ResultFiles.GetData() + k;
	}
}





// UINT_PTR __stdcall OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
// BOOL get_file_name(char* title, char* filter, char* buffer)
// {
// 	OPENFILENAME ofn = { 0 };
// 	*buffer = 0;
// 	ofn.lStructSize = sizeof(ofn);
// 	ofn.hInstance = GetModuleHandle(NULL);
// 	//允许 缩放窗口+资源管理器风格+文件必须存在+隐藏只读文件+使能钩子+使能模板
// 	ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
// 	ofn.lpstrFilter = filter;
// 	ofn.lpstrFile = &buffer[0];
// 	ofn.nMaxFile = MAX_PATH;
// 	ofn.lpstrTitle = title;
// 	ofn.lpfnHook = OFNHookProc;
// 	ofn.lpTemplateName = MAKEINTRESOURCE(IDD_DLG_TEMPLATE);
// 	return (BOOL)GetOpenFileName(&ofn);
// }

/************************************************************************
函数:OFNHookProc@16
功能:GetOpenFileName/GetSaveFileName的子类/钩子处理过程
参数:   hdlg - 该对话框模板句柄, 注意:不是"打开/保存"对话框的句柄
uiMsg,wParam,lParam - 同常规Windows消息参数
返回:   由于是对话框消息,所以返回值应使用SetWindowLong(...,DWL_MSGRESULT,...)来返回
分以下3种情况返回:
1.钩子函数返回0:默认对话框函数继续处理该消息
2.钩子函数返回非0:默认对话框函数忽略该消息不再处理
3.(例外)对于CDN_SHAREVIOLATION 和 CDN_FILEOK(点击"打开/保存"时触发) 通知消
息时,钩子过程应该明确返回一个非0值以指示钩子过程已经使用SetWindowLong(...,DWL_MSGRESULT,...);
设置了一个非零的该消息的返回值, 默认对话框函数不再继续处理
*************************************************************************/
// UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
// {
// 	static HWND hParent;    //对话框模板的父窗口句柄
// 	static HWND hCombo;     //我增加的ComboBox的句柄
// 	static HWND hStatic;    //我增加的Static控件的句柄
// 	if (uiMsg == WM_NOTIFY) {
// 		LPOFNOTIFY pofn = (LPOFNOTIFY)lParam;
// 		if (pofn->hdr.code == CDN_FILEOK) {
// 			//返回0-关闭对话框并返回
// 			//返回!0-禁止关闭对话框
// 			int iSel = ComboBox_GetCurSel(hCombo);
// 			if (iSel != 1) {//未选择"测试字符串二"时
// 				MessageBox(hParent, "你必须选择\"测试字符串二\"才能离开!", NULL, MB_ICONEXCLAMATION);
// 				SetWindowLong(hdlg, DWL_MSGRESULT, 1);
// 				//或使用:SetDlgMsgResult(hdlg,1);
// 				return 1;
// 			}
// 			return 0;
// 		}
// 	}
// 	else if (uiMsg == WM_SIZE) {
// 
// 		HWND hCboCurFlt = GetDlgItem(hParent, cmb1);    //过虑列表ComboBox
// 		HWND hStaticFlt = GetDlgItem(hParent, stc2);     //过虑列表左侧的Static
// 		HWND hCboFile = GetDlgItem(hParent, cmb13);      //当前选择的文件ComboBox
// 		RECT rcCboFlt, rcStaFlt, rcDlg, rcFile;
// 		int left, top, width, height;
// 		//这个矩形是"过虑ComboBox"和"过虑提示Static控件",我们根据它们来对齐控件
// 		GetWindowRect(hCboCurFlt, &rcCboFlt);
// 		GetWindowRect(hStaticFlt, &rcStaFlt);
// 		//这个是"文件名(Edit)"-当前选择的, 用来计算它和过虑的间距,以调整我的控件和过虑Combo的间距
// 		GetWindowRect(hCboFile, &rcFile);
// 		//说实话,我并不清楚对话框模板的位置是怎样的,反正左边距不是0,不清楚
// 		//所以,调整窗口的时候要相对移动坐标
// 		GetWindowRect(hdlg, &rcDlg);
// 
// 		//设定坐标, 注意, 这里的坐标不是相对于我们的对话框模板的,而是"打开/关闭",好好理解下
// 		//什么时候能画个图示意下就好了
// 		left = rcCboFlt.left - rcDlg.left;
// 		top = rcCboFlt.top - rcDlg.top + (rcCboFlt.top - rcFile.top);
// 		width = rcCboFlt.right - rcCboFlt.left;
// 		height = rcCboFlt.bottom - rcCboFlt.top;
// 		SetWindowPos(hCombo, 0, left, top, width, height, SWP_NOZORDER);
// 
// 		left = rcStaFlt.left - rcDlg.left;
// 		top = rcStaFlt.top - rcDlg.top + (rcCboFlt.top - rcFile.top);
// 		width = rcStaFlt.right - rcStaFlt.left;
// 		height = rcStaFlt.bottom - rcStaFlt.top;
// 		SetWindowPos(hStatic, 0, left, top, width, height, SWP_NOZORDER);
// 		return 0;
// 	}
// 	else if (uiMsg == WM_INITDIALOG) {
// 		HFONT hFontDialog = NULL;
// 
// 		hParent = GetParent(hdlg);
// 		//初始化我们自己的句柄,方便下次使用
// 		hCombo = GetDlgItem(hdlg, IDC_COMBO_TEST);
// 		hStatic = GetDlgItem(hdlg, IDC_STATIC_TEST);
// 		//初始化我们的控件的数据
// 		ComboBox_AddString(hCombo, "测试字符串一");
// 		ComboBox_AddString(hCombo, "测试字符串二");
// 		ComboBox_AddString(hCombo, "女孩不哭,哈哈!");
// 		ComboBox_SetCurSel(hCombo, 0);
// 		//一般为了表现效果一致,需要设置一下字体,比如说VC6的默认字体
// 		//System就相当的丑陋,如果不修改的话,受不了...
// 		//当然,最好是采用"打开"对话框的统一字体
// 		//如果你当前也使用VC6,可以注释掉看看,呃.........
// 		hFontDialog = (HFONT)SendMessage(hParent, WM_GETFONT, 0, 0);
// 		SendMessage(hCombo, WM_SETFONT, (WPARAM)hFontDialog, MAKELPARAM(TRUE, 0));
// 		SendMessage(hStatic, WM_SETFONT, (WPARAM)hFontDialog, MAKELPARAM(TRUE, 0));
// 		return 0;
// 	}
// 	UNREFERENCED_PARAMETER(wParam);
// 	return 0;
// }

