#pragma once
#include "ErasureThread.h"


class LDLIB_API CErasureVolumeUI : 
	public CFramWnd, 
	IThreadRunable,                      //�ļ������߳�ִ�д���
	IGernalCallback<LPVOID>,             //�������ɾ��ListUI�У�Send2MainThread��   
	IGernalCallback<CLdArray<TCHAR*>*>,  //����վ��ʾ�ļ�
	IGernalCallback<PSH_HEAD_INFO>,      //����վ�ļ���ʾ����Ϣ
	IGernalCallback<TCHAR*>              //ö�ٴ��̣�����·����
{
public:
	CErasureVolumeUI();
	virtual ~CErasureVolumeUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOk;
	CListUI* lstVolume;
	CLdArray<PSH_HEAD_INFO> m_Columes;   //ShellView ��ͷ
	CLdHashMap<PERASURE_FILE_DATA> m_ErasureFiles; //����վ�е�ʵ���ļ�����ʾ��UI�еĲ��������ļ���
	CEreaserThrads m_EreaserThreads;  //��������߳�

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);

protected:
	virtual void OnInit() override;
	void AddLstViewHeader(int ncount);
	//���߳���ɾ���Ѳ������ļ���ӦlstFile��
	BOOL GernalCallback_Callback(LPVOID pData, UINT_PTR Param) override;
	//ShGetFileInfo ����վ��ʾ�ļ�
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//ö��ShellView��ͷ��Ϣ
	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override;
	//FindFirstVolume ö�ٴ��̣�����·����
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;

	void ErasureAllFiles(CThread* Sender);
	void ErasureSingleFile(CThread* Sender, TCHAR* Key);

	virtual VOID ThreadRun(CThread* Sender, UINT_PTR Param) override;
	virtual VOID OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	virtual VOID OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;

};

