#pragma once
#include "../eraser/ErasureThread.h"
#include "ShFileView.h"

class CErasureFileUI : 
	IEraserThreadCallback,  //�ļ������̻߳ص��������������״̬��������Ϣ��
	public CShFileViewUI
{
public:
	CErasureFileUI();
	virtual ~CErasureFileUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	typedef struct FILE_ERASURE_DATA
	{
		E_FILE_STATE nStatus;             //����״̬
		DWORD        nErrorCode;          //������루�������
		CControlUI* ui;                   //listView ��
		DWORD nCount;                     //�ļ����µ��ܵ��ļ���
		DWORD nErasued;                   //�Ѿ����������ļ���
	}*PFILE_ERASURE_DATA;

	CButtonUI* btnOpenFile;
	CButtonUI* btnOk;
	CEreaserThrads m_EreaserThreads;
	CFolderInfo m_ErasureFile;

	void addFileUI(CVirtualFile* pFile);
	virtual void OnClick(DuiLib::TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);
	DWORD SetFolderFilesData(CVirtualFile* pFile);
	bool OnAfterColumePaint(PVOID Param);
protected:
	CVirtualFile* AddEraseFile(TCHAR* file_name);
	void AttanchControl(CControlUI* pCtrl) override;
	void DeleteErasuredFile(CLdArray<CVirtualFile*>* files);
	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;
	void StatErase();
};

