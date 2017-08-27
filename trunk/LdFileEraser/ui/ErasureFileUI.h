/*******************************************************
�����ļ���Ŀ¼����
���������ļ����ļ��С�
********************************************************/
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
	CEreaserThrads m_EreaserThreads;        //���������߳�

	virtual void OnClick(DuiLib::TNotifyUI& msg);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);      //�˳�ʱ����ļ�����
	DWORD SetFolderFilesData(CVirtualFile* pFile, CControlUI* ui);            //���ļ��������ݣ���¼�ļ�����״̬����Ϣ
protected:

	typedef struct FILE_ERASURE_DATA
	{
		E_FILE_STATE nStatus;             //����״̬
		DWORD        nErrorCode;          //������루�������
		CControlUI* ui;                   //listView ��
		DWORD nCount;                     //�ļ����µ��ܵ��ļ���
		DWORD nErasued;                   //�Ѿ����������ļ���
	}*PFILE_ERASURE_DATA;

	CFolderInfo m_ErasureFile;                     //Ҫ�������ļ���������
	CVirtualFile* AddEraseFile(TCHAR* file_name);  //��Ӵ��������ļ�
	void AddFileUI(CVirtualFile* pFile, CLdArray<TCHAR*>* pColumeData = nullptr);  //���ļ���Ϣ��ʾ��ListUI��
	bool GetViewHeader() override;                                                 //ListUI�����ͷ��ȡWindows��Դ������������Ϣ��

	bool OnAfterColumePaint(PVOID Param);                                          //������Paint�¼������е���������
	void AttanchControl(CControlUI* pCtrl) override;                   
	void DeleteErasuredFile(CLdArray<CVirtualFile*>* files);                       //ɾ���Ѿ���������ɵļ�¼��
	void UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui, int Percent);            //���²�����Ϣ����ʾ��ListUI�У�
	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;    //�����̵߳Ļص�����
	void StatErase();        //��ʼ����
};

