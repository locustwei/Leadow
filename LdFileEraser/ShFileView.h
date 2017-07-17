#pragma once
#include <minwindef.h>


class LDLIB_API CShFileViewUI : 
	public CFramWnd, 
	IGernalCallback<CLdArray<TCHAR*>*>,  	
	IGernalCallback<PSH_HEAD_INFO>     
{
public:
	CShFileViewUI();
	virtual ~CShFileViewUI() override;

	void AddRecord(CLdArray<TCHAR*>* values);
	DWORD AddFile(TCHAR* lpFullName);
	DWORD AddFolder(TCHAR* lpFullName);
	DWORD AddFolder(int nFolderCSIDL);

	DUI_DECLARE_MESSAGE_MAP()

private:
	boolean m_HeaderAdded;
	TCHAR* m_ItemSkin;

	CListUI* lstFile;
	CLdArray<PSH_HEAD_INFO> m_Columes;   //ShellView ��ͷ

	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);

protected:
	virtual void OnInit() override;
	void AddLstViewHeader(int ncount);
	//ShGetFileInfo ����վ��ʾ�ļ�
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//ö��ShellView��ͷ��Ϣ
	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override;
};

