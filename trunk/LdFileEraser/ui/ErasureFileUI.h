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
	typedef struct FILE_ERASURE_DATA
	{
		E_FILE_STATE nStatus;             //擦除状态
		DWORD        nErrorCode;          //错误代码（如果错误）
		CControlUI* ui;                   //listView 行
		DWORD nCount;                     //文件夹下的总的文件数
		DWORD nErasued;                   //已经被擦除的文件数
	}*PFILE_ERASURE_DATA;

	CButtonUI* btnOpenFile;
	CButtonUI* btnOk;
	CEreaserThrads m_EreaserThreads;
	CLdArray<CVirtualFile*> m_ErasureFiles;

	virtual void OnClick(DuiLib::TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
protected:
	CVirtualFile* AddEraseFile(TCHAR* file_name);
	void AttanchControl(CControlUI* pCtrl) override;
	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;

};

