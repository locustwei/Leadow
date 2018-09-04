#pragma once

/*******************************************************
��Windows��Դ��������Shell����ʽ����ʾ�ļ����ļ��У���Ϣ
********************************************************/

class CShFileViewUI : 
	public CFramNotifyPump, 
	//public ICommunicationListen,
	IGernalCallback<CLdArray<TCHAR*>*>,  	 //CSHFolders::EnumFolderObjects �ص�
	public IGernalCallback<PSH_HEAD_INFO>           //CSHFolders::EnumFolderColumes �ص�
{
public:
	CShFileViewUI();
	~CShFileViewUI() override;

	//��һ��
	CControlUI* AddRecord(CLdArray<TCHAR*>* values);
	//��һ���ļ���CSHFolders::GetFileAttributeValue��
	CControlUI* AddFile(TCHAR* lpFullName);
	//���Ŀ¼��CSHFolders::EnumFolderObjects��
	DWORD AddFolder(TCHAR* lpFullName);
	//�������Ŀ¼
	DWORD AddFolder(int nFolderCSIDL);

	DUI_DECLARE_MESSAGE_MAP()

private:
	boolean m_HeaderAdded;

	CLdArray<PSH_HEAD_INFO> m_Columes;   //ShellView ��ͷ

protected:
	CListUI* lstFile;
	TCHAR* m_ItemSkin;

	void AttanchControl(CControlUI* pCtrl) override;

	virtual bool GetViewHeader() = 0;
	void AddLstViewHeader(int ncount);
	//ShGetFileInfo �������Ŀ¼�ص�pData�ļ�����ֵ
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//ö��ShellView��ͷ��Ϣ
	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override;
//	bool OnCreate() override;
//	void OnTerminate(DWORD exitcode) override;
//	void OnCommand(WORD id, PVOID data, WORD nSize) override;
};

