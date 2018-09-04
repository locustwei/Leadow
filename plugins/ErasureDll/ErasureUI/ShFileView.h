#pragma once

/*******************************************************
用Windows资源管理器（Shell）格式，显示文件（文件夹）信息
********************************************************/

class CShFileViewUI : 
	public CFramNotifyPump, 
	//public ICommunicationListen,
	IGernalCallback<CLdArray<TCHAR*>*>,  	 //CSHFolders::EnumFolderObjects 回掉
	public IGernalCallback<PSH_HEAD_INFO>           //CSHFolders::EnumFolderColumes 回掉
{
public:
	CShFileViewUI();
	~CShFileViewUI() override;

	//加一行
	CControlUI* AddRecord(CLdArray<TCHAR*>* values);
	//加一个文件（CSHFolders::GetFileAttributeValue）
	CControlUI* AddFile(TCHAR* lpFullName);
	//添加目录（CSHFolders::EnumFolderObjects）
	DWORD AddFolder(TCHAR* lpFullName);
	//添加特殊目录
	DWORD AddFolder(int nFolderCSIDL);

	DUI_DECLARE_MESSAGE_MAP()

private:
	boolean m_HeaderAdded;

	CLdArray<PSH_HEAD_INFO> m_Columes;   //ShellView 列头

protected:
	CListUI* lstFile;
	TCHAR* m_ItemSkin;

	void AttanchControl(CControlUI* pCtrl) override;

	virtual bool GetViewHeader() = 0;
	void AddLstViewHeader(int ncount);
	//ShGetFileInfo 添加特殊目录回调pData文件属性值
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//枚举ShellView列头信息
	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override;
//	bool OnCreate() override;
//	void OnTerminate(DWORD exitcode) override;
//	void OnCommand(WORD id, PVOID data, WORD nSize) override;
};

