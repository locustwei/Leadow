
#pragma once
/*******************************************************
�����ļ���Ŀ¼����
���������ļ����ļ��С�
********************************************************/

#include "ShFileView.h"
#include "../define.h"
#include "LdCommunication.h"

class CErasureFileUI : 
	//IEraserListen,  //�ļ������̻߳ص��������������״̬��������Ϣ��
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
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);               //�˳�ʱ����ļ�����
	DWORD SetFolderFilesData(CVirtualFile* pFile, CControlUI* ui);     //���ļ��������ݣ���¼�ļ�����״̬����Ϣ
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
	CLdArray<PERASER_FILE_DATA> m_ErasureFile;                     //Ҫ�������ļ���������
	//CLdMap<CLdString*, CControlUI*> m_file_map;
	//CVirtualFile* AddEraseFile(TCHAR* file_name);  //��Ӵ��������ļ�
	void AddFileUI(CDynObject);  //���ļ���Ϣ��ʾ��ListUI��
	bool GetViewHeader() override;                                                 //ListUI�����ͷ��ȡWindows��Դ������������Ϣ��
	bool OnAfterColumePaint(PVOID Param);                                          //������Paint�¼������е���������
	void AttanchControl(CControlUI* pCtrl) override;                   
	//void DeleteErasuredFile(CVirtualFile* pFile);                       //ɾ���Ѿ���������ɵļ�¼��
	void UpdateEraseProgressMsg(PERASER_FILE_DATA pData, bool bRoot);
	PERASER_FILE_DATA GetFileData(TCHAR* pFile);
	//���²�����Ϣ����ʾ��ListUI�У�
	//bool EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue) ;    //�����̵߳Ļص�����
	void StatErase();        //��ʼ����
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
