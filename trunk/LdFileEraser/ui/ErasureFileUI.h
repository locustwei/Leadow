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

	CButtonUI* btnOpenFile;
	CButtonUI* btnOk;
	CEreaserThrads m_EreaserThreads;

	virtual void OnClick(DuiLib::TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);
	DWORD SetFolderFilesData(CVirtualFile* pFile);
protected:
	typedef struct FILE_ERASURE_DATA
	{
		E_FILE_STATE nStatus;             //����״̬
		DWORD        nErrorCode;          //������루�������
		CControlUI* ui;                   //listView ��
		DWORD nCount;                     //�ļ����µ��ܵ��ļ���
		DWORD nErasued;                   //�Ѿ����������ļ���
	}*PFILE_ERASURE_DATA;

	CFolderInfo m_ErasureFile;

	CVirtualFile* AddEraseFile(TCHAR* file_name);
	void AddFileUI(CVirtualFile* pFile, CLdArray<TCHAR*>* pColumeData = nullptr);

	bool OnAfterColumePaint(PVOID Param);
	void AttanchControl(CControlUI* pCtrl) override;
	void DeleteErasuredFile(CLdArray<CVirtualFile*>* files);
	void UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui, int Percent);
	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;
	void StatErase();
};

