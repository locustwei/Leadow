
#pragma once
/*******************************************************
擦除文件、目录窗口
添加需擦除文件和文件夹。
********************************************************/

#include "ShFileView.h"
#include "../define.h"
#include "LdCommunication.h"

class CErasureFileUI : 
	//IEraserListen,  //文件擦除线程回掉函数，报告擦除状态、进度信息。
	ICommunicationListen,
	public CShFileViewUI
{
public:
	CErasureFileUI();
	virtual ~CErasureFileUI() override;

	DUI_DECLARE_MESSAGE_MAP()
private:
	CLdCommunication* m_Comm;

	CButtonUI* btnOpenFile;
	bool m_Abort;

	virtual void OnClick(DuiLib::TNotifyUI& msg);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);               //退出时清除文件对象。
	DWORD SetFolderFilesData(CVirtualFile* pFile, CControlUI* ui);     //给文件附加数据，记录文件擦除状态等信息
	void ExecuteFileAnalysis(CLdArray<TCHAR*>* filenames);
protected:
	typedef struct ERASER_FILE_DATA
	{
		~ERASER_FILE_DATA()
		{
			if(filename)
				delete filename;
		};
		CLdString* filename;
		DWORD nFileCount;
		DWORD nErased;
		DWORD nError;
		CControlUI* ui;
	}*PERASER_FILE_DATA;

	CButtonUI* btnOk;
	CLdArray<PERASER_FILE_DATA> m_ErasureFile;                     //要擦除的文件放在这里
	//CLdMap<CLdString*, CControlUI*> m_file_map;
	//CVirtualFile* AddEraseFile(TCHAR* file_name);  //添加待擦除的文件
	void AddFileUI(CDynObject);  //在文件信息显示在ListUI中
	bool GetViewHeader() override;                                                 //ListUI添加列头（取Windows资源管理器的列信息）
	bool OnAfterColumePaint(PVOID Param);                                          //处理列Paint事件，把列当进度条用
	void AttanchControl(CControlUI* pCtrl) override;                   
	//void DeleteErasuredFile(CVirtualFile* pFile);                       //删除已经被擦除完成的记录。
	void UpdateEraseProgressMsg(PERASER_FILE_DATA pData, bool bRoot);
	PERASER_FILE_DATA GetFileData(TCHAR* pFile);
	//更新擦除信息（显示在ListUI中）
	//bool EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue) ;    //擦除线程的回掉函数
	void StatErase();        //开始擦除
	//bool AnalyResult(TCHAR* FileName, PVOID pData);
	//bool IsSelecteFile(TCHAR*);

protected: //ICommunicationListen
	bool OnCreate() override;
	void OnTerminate(DWORD exitcode) override;
	void OnCommand(WORD id, CDynObject& Param) override;
private:
	void OnAnaResult(CDynObject& files);
	void OnEraseFileStatus(CDynObject& fileStatus);
};
