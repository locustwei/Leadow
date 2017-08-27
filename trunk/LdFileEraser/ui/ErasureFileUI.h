/*******************************************************
擦除文件、目录窗口
添加需擦除文件和文件夹。
********************************************************/
#pragma once

#include "../eraser/ErasureThread.h"
#include "ShFileView.h"

class CErasureFileUI : 
	IEraserThreadCallback,  //文件擦除线程回掉函数，报告擦除状态、进度信息。
	public CShFileViewUI
{
public:
	CErasureFileUI();
	virtual ~CErasureFileUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:

	CButtonUI* btnOpenFile;
	CButtonUI* btnOk;
	CEreaserThrads m_EreaserThreads;        //擦除调度线程

	virtual void OnClick(DuiLib::TNotifyUI& msg);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);      //退出时清除文件对象。
	DWORD SetFolderFilesData(CVirtualFile* pFile, CControlUI* ui);            //给文件附加数据，记录文件擦除状态等信息
protected:

	typedef struct FILE_ERASURE_DATA
	{
		E_FILE_STATE nStatus;             //擦除状态
		DWORD        nErrorCode;          //错误代码（如果错误）
		CControlUI* ui;                   //listView 行
		DWORD nCount;                     //文件夹下的总的文件数
		DWORD nErasued;                   //已经被擦除的文件数
	}*PFILE_ERASURE_DATA;

	CFolderInfo m_ErasureFile;                     //要擦除的文件放在这里
	CVirtualFile* AddEraseFile(TCHAR* file_name);  //添加待擦除的文件
	void AddFileUI(CVirtualFile* pFile, CLdArray<TCHAR*>* pColumeData = nullptr);  //在文件信息显示在ListUI中
	bool GetViewHeader() override;                                                 //ListUI添加列头（取Windows资源管理器的列信息）

	bool OnAfterColumePaint(PVOID Param);                                          //处理列Paint事件，把列当进度条用
	void AttanchControl(CControlUI* pCtrl) override;                   
	void DeleteErasuredFile(CLdArray<CVirtualFile*>* files);                       //删除已经被擦除完成的记录。
	void UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui, int Percent);            //更新擦除信息（显示在ListUI中）
	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;    //擦除线程的回掉函数
	void StatErase();        //开始擦除
};

