
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

	CButtonUI* btnOk;
	CFolderInfo m_ErasureFile;                     //Ҫ�������ļ���������
	//CLdMap<CLdString*, CControlUI*> m_file_map;
	CVirtualFile* AddEraseFile(TCHAR* file_name);  //��Ӵ��������ļ�
	void AddFileUI(CVirtualFile* pFile, CLdArray<TCHAR*>* pColumeData = nullptr);  //���ļ���Ϣ��ʾ��ListUI��
	bool GetViewHeader() override;                                                 //ListUI�����ͷ��ȡWindows��Դ������������Ϣ��

	bool OnAfterColumePaint(PVOID Param);                                          //������Paint�¼������е���������
	void AttanchControl(CControlUI* pCtrl) override;                   
	void DeleteErasuredFile(CLdArray<CVirtualFile*>* files);                       //ɾ���Ѿ���������ɵļ�¼��
	void UpdateEraseProgressMsg(CControlUI* ui, int Percent);            //���²�����Ϣ����ʾ��ListUI�У�
	bool EraserThreadCallback(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue) ;    //�����̵߳Ļص�����
	void StatErase();        //��ʼ����
};
