#pragma once


class LDLIB_API CShFileViewUI : 
	public CFramNotifyPump, 
	IGernalCallback<CLdArray<TCHAR*>*>,  	 //CSHFolders::EnumFolderObjects �ص�
	public IGernalCallback<PSH_HEAD_INFO>           //CSHFolders::EnumFolderColumes �ص�
{
public:
	CShFileViewUI();
	virtual ~CShFileViewUI() override;

	//��һ��
	CControlUI* AddRecord(CLdArray<TCHAR*>* values);
	//��һ���ļ���CSHFolders::GetFileAttributeValue��
	DWORD AddFile(TCHAR* lpFullName);
	//���Ŀ¼��CSHFolders::EnumFolderObjects��
	DWORD AddFolder(TCHAR* lpFullName);
	//�������Ŀ¼
	DWORD AddFolder(int nFolderCSIDL);

	DUI_DECLARE_MESSAGE_MAP()

private:
	boolean m_HeaderAdded;

	CLdArray<PSH_HEAD_INFO> m_Columes;   //ShellView ��ͷ

	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);

protected:
	CListUI* lstFile;
	TCHAR* m_ItemSkin;

	void AttanchControl(CControlUI* pCtrl);

	void AddLstViewHeader(int ncount);
	//ShGetFileInfo ����վ��ʾ�ļ�
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//ö��ShellView��ͷ��Ϣ
	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override;
};

