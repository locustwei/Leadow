#pragma once
#include "ErasureThread.h"

class LDLIB_API CErasureFileUI : 
	public CFramWnd, 
	IThreadRunable,                      //�ļ������߳�ִ�д���
	IGernalCallback<LPVOID>,             //�������ɾ��ListUI�У�Send2MainThread��   
	IGernalCallback<LPWIN32_FIND_DATA>,  //����վʵ���ļ�
	IGernalCallback<CLdArray<TCHAR*>*>,  //����վ��ʾ�ļ�
	IGernalCallback<PSH_HEAD_INFO>,      //����վ�ļ���ʾ����Ϣ
	IGernalCallback<TCHAR*>              //ö�ٴ��̣�����·����
{
public:
	CErasureFileUI();
	virtual ~CErasureFileUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOpenFile;
	CButtonUI* btnOk;
	CListUI* lstFile;
	CEreaserThrads m_EreaserThreads;

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	void AddErasureFile(CLdString& filename);

	void ErasureAllFiles(CThread* Sender);
	void ErasureSingleFile(CThread* Sender, TCHAR* Key);
protected:
	virtual void OnInit() override;
	
	virtual VOID ThreadRun(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param) override;

};

