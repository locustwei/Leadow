
#pragma once
/*******************************************************
擦除文件、目录窗口
添加需擦除文件和文件夹。
********************************************************/

#include "ShFileView.h"
#include "../define.h"

class CErasureFileUI : 
	//IEraserListen,  //文件擦除线程回掉函数，报告擦除状态、进度信息。
	//ICommunicationListen,
	public CShFileViewUI
{
public:
	CErasureFileUI();
	virtual ~CErasureFileUI() override;

	DUI_DECLARE_MESSAGE_MAP()
private:
	CFileEraserComm* m_Comm;

	CButtonUI* btnOpenFile;
	bool m_Abort;

	virtual void OnClick(DuiLib::TNotifyUI& msg);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);               //退出时清除文件对象。
	DWORD SetFolderFilesData(CVirtualFile* pFile, CControlUI* ui);     //给文件附加数据，记录文件擦除状态等信息
protected:
	typedef struct _FILE_EX_DATA
	{
		DWORD nFileCount;
		DWORD nErased;
		DWORD nError;
		CControlUI* ui;
	}FILE_EX_DATA, *PFILE_EX_DATA;

	CButtonUI* btnOk;
	CFolderInfo m_ErasureFile;                     //要擦除的文件放在这里
	//CLdMap<CLdString*, CControlUI*> m_file_map;
	CVirtualFile* AddEraseFile(TCHAR* file_name);  //添加待擦除的文件
	void AddFileUI(CVirtualFile* pFile);  //在文件信息显示在ListUI中
	bool GetViewHeader() override;                                                 //ListUI添加列头（取Windows资源管理器的列信息）
	bool OnAfterColumePaint(PVOID Param);                                          //处理列Paint事件，把列当进度条用
	void AttanchControl(CControlUI* pCtrl) override;                   
	void DeleteErasuredFile(CVirtualFile* pFile);                       //删除已经被擦除完成的记录。
	void UpdateEraseProgressMsg(PFILE_EX_DATA pData, bool bRoot);
	PFILE_EX_DATA GetFileData(CVirtualFile* pFile);
	//更新擦除信息（显示在ListUI中）
	bool EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue) ;    //擦除线程的回掉函数
	void StatErase();        //开始擦除
	bool AnalyResult(TCHAR* FileName, PVOID pData);
protected: //ICommunicationListen
	bool OnCreate() ;
	void OnTerminate(DWORD exitcode) ;
	void OnCommand(WORD id, PVOID data, WORD nSize) ;
};
