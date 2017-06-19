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
		return m_Files.GetCount() - 1;  //��0����·��
	else
		return m_Files.GetCount();
}

CLdString CDlgGetFileName::GetFileName(int index, BOOL bWithPath)
{
	if (index > GetFileCount() - 1)
		return _T("");
	if (GetFileCount() > 1)
	{ //��ѡ
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
// 	//���� ���Ŵ���+��Դ���������+�ļ��������+����ֻ���ļ�+ʹ�ܹ���+ʹ��ģ��
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
����:OFNHookProc@16
����:GetOpenFileName/GetSaveFileName������/���Ӵ������
����:   hdlg - �öԻ���ģ����, ע��:����"��/����"�Ի���ľ��
uiMsg,wParam,lParam - ͬ����Windows��Ϣ����
����:   �����ǶԻ�����Ϣ,���Է���ֵӦʹ��SetWindowLong(...,DWL_MSGRESULT,...)������
������3���������:
1.���Ӻ�������0:Ĭ�϶Ի����������������Ϣ
2.���Ӻ������ط�0:Ĭ�϶Ի��������Ը���Ϣ���ٴ���
3.(����)����CDN_SHAREVIOLATION �� CDN_FILEOK(���"��/����"ʱ����) ֪ͨ��
Ϣʱ,���ӹ���Ӧ����ȷ����һ����0ֵ��ָʾ���ӹ����Ѿ�ʹ��SetWindowLong(...,DWL_MSGRESULT,...);
������һ������ĸ���Ϣ�ķ���ֵ, Ĭ�϶Ի��������ټ�������
*************************************************************************/
// UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
// {
// 	static HWND hParent;    //�Ի���ģ��ĸ����ھ��
// 	static HWND hCombo;     //�����ӵ�ComboBox�ľ��
// 	static HWND hStatic;    //�����ӵ�Static�ؼ��ľ��
// 	if (uiMsg == WM_NOTIFY) {
// 		LPOFNOTIFY pofn = (LPOFNOTIFY)lParam;
// 		if (pofn->hdr.code == CDN_FILEOK) {
// 			//����0-�رնԻ��򲢷���
// 			//����!0-��ֹ�رնԻ���
// 			int iSel = ComboBox_GetCurSel(hCombo);
// 			if (iSel != 1) {//δѡ��"�����ַ�����"ʱ
// 				MessageBox(hParent, "�����ѡ��\"�����ַ�����\"�����뿪!", NULL, MB_ICONEXCLAMATION);
// 				SetWindowLong(hdlg, DWL_MSGRESULT, 1);
// 				//��ʹ��:SetDlgMsgResult(hdlg,1);
// 				return 1;
// 			}
// 			return 0;
// 		}
// 	}
// 	else if (uiMsg == WM_SIZE) {
// 
// 		HWND hCboCurFlt = GetDlgItem(hParent, cmb1);    //�����б�ComboBox
// 		HWND hStaticFlt = GetDlgItem(hParent, stc2);     //�����б�����Static
// 		HWND hCboFile = GetDlgItem(hParent, cmb13);      //��ǰѡ����ļ�ComboBox
// 		RECT rcCboFlt, rcStaFlt, rcDlg, rcFile;
// 		int left, top, width, height;
// 		//���������"����ComboBox"��"������ʾStatic�ؼ�",���Ǹ�������������ؼ�
// 		GetWindowRect(hCboCurFlt, &rcCboFlt);
// 		GetWindowRect(hStaticFlt, &rcStaFlt);
// 		//�����"�ļ���(Edit)"-��ǰѡ���, �����������͹��ǵļ��,�Ե����ҵĿؼ��͹���Combo�ļ��
// 		GetWindowRect(hCboFile, &rcFile);
// 		//˵ʵ��,�Ҳ�������Ի���ģ���λ����������,������߾಻��0,�����
// 		//����,�������ڵ�ʱ��Ҫ����ƶ�����
// 		GetWindowRect(hdlg, &rcDlg);
// 
// 		//�趨����, ע��, ��������겻����������ǵĶԻ���ģ���,����"��/�ر�",�ú������
// 		//ʲôʱ���ܻ���ͼʾ���¾ͺ���
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
// 		//��ʼ�������Լ��ľ��,�����´�ʹ��
// 		hCombo = GetDlgItem(hdlg, IDC_COMBO_TEST);
// 		hStatic = GetDlgItem(hdlg, IDC_STATIC_TEST);
// 		//��ʼ�����ǵĿؼ�������
// 		ComboBox_AddString(hCombo, "�����ַ���һ");
// 		ComboBox_AddString(hCombo, "�����ַ�����");
// 		ComboBox_AddString(hCombo, "Ů������,����!");
// 		ComboBox_SetCurSel(hCombo, 0);
// 		//һ��Ϊ�˱���Ч��һ��,��Ҫ����һ������,����˵VC6��Ĭ������
// 		//System���൱�ĳ�ª,������޸ĵĻ�,�ܲ���...
// 		//��Ȼ,����ǲ���"��"�Ի����ͳһ����
// 		//����㵱ǰҲʹ��VC6,����ע�͵�����,��.........
// 		hFontDialog = (HFONT)SendMessage(hParent, WM_GETFONT, 0, 0);
// 		SendMessage(hCombo, WM_SETFONT, (WPARAM)hFontDialog, MAKELPARAM(TRUE, 0));
// 		SendMessage(hStatic, WM_SETFONT, (WPARAM)hFontDialog, MAKELPARAM(TRUE, 0));
// 		return 0;
// 	}
// 	UNREFERENCED_PARAMETER(wParam);
// 	return 0;
// }

