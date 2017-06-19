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

BOOL CDlgGetFileName::OpenFile(HWND hOwner)
{
	BOOL Result = FALSE;
	
	Result = GetOpenFileName(PrepareParam(hOwner));

	if (Result)
	{
		ProcessResult();
	}
	return Result;

}

BOOL CDlgGetFileName::SaveFile(HWND hOwner)
{
	BOOL result = GetSaveFileName(PrepareParam(hOwner));
	if (result)
		ProcessResult();
	return result;
}

VOID CDlgGetFileName::SetDefaultExt(LPCTSTR lpExt)
{
	m_DefaultExt = lpExt;
}

VOID CDlgGetFileName::SetDefaultName(LPCTSTR lpFileName)
{
	m_DefaultName = lpFileName;
}

VOID CDlgGetFileName::AddFilter(LPCTSTR lpFilter)
{
	m_Filters.Add(lpFilter);
}

VOID CDlgGetFileName::SetOption(DWORD dwOption)
{
	m_Option = dwOption;
}

VOID CDlgGetFileName::SetInitDir(LPCTSTR lpFolder)
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

LPCTSTR CDlgGetFileName::GetFilterStr()
{
	memset(m_FilterTmp.GetData(), 0, MAX_PATH * sizeof(TCHAR));
	int k = 0;
	TCHAR* p = m_FilterTmp;

	for (int i = 0; i < m_Filters.GetCount(); i++)
	{
		CLdString s = m_Filters[i];
		if(s.GetLength() == 0)
			continue;
		s.CopyTo(p);
		p += s.GetLength() + 1;
		k += s.GetLength() + 1;
	}

	for (int i = 0; i < k; i++)
	{
		if (m_FilterTmp[i] == '|')
			m_FilterTmp.SetAt(i, '\0');
	}
	return m_FilterTmp.GetData();
}

OPENFILENAME * CDlgGetFileName::PrepareParam(HWND hOwner)
{
	HWND hwnd = NULL;
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
	int k = 0;
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

