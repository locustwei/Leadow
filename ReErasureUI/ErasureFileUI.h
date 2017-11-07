
#pragma once
/*******************************************************
�����ļ���Ŀ¼����
���������ļ����ļ��С�
********************************************************/
//#include "../eraser/ErasureThread.h"
#include "ShFileView.h"
#include "../LdApp/LdStructs.h"

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
	//CEreaserThrads m_EreaserThreads;                                    //���������߳�

	virtual void OnClick(DuiLib::TNotifyUI& msg);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);               //�˳�ʱ����ļ�����
	DWORD SetFolderFilesData(CVirtualFile* pFile, CControlUI* ui);     //���ļ��������ݣ���¼�ļ�����״̬����Ϣ
protected:
	typedef struct _FILE_EX_DATA
	{
		DWORD nFileCount;
		DWORD nErased;
		DWORD nError;
		CControlUI* ui;
	}FILE_EX_DATA, *PFILE_EX_DATA;

	CButtonUI* btnOk;
	CFolderInfo m_ErasureFile;                     //Ҫ�������ļ���������
	//CLdMap<CLdString*, CControlUI*> m_file_map;
	CVirtualFile* AddEraseFile(TCHAR* file_name);  //��Ӵ��������ļ�
	void AddFileUI(CVirtualFile* pFile);  //���ļ���Ϣ��ʾ��ListUI��
	bool GetViewHeader() override;                                                 //ListUI�����ͷ��ȡWindows��Դ������������Ϣ��

	bool OnAfterColumePaint(PVOID Param);                                          //������Paint�¼������е���������
	void AttanchControl(CControlUI* pCtrl) override;                   
	void DeleteErasuredFile(CVirtualFile* pFile);                       //ɾ���Ѿ���������ɵļ�¼��
	void UpdateEraseProgressMsg(PFILE_EX_DATA pData);
	PFILE_EX_DATA GetFileData(CVirtualFile* pFile);
	//���²�����Ϣ����ʾ��ListUI�У�
	bool EraserThreadCallback(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue) ;    //�����̵߳Ļص�����
	void StatErase();        //��ʼ����
};
