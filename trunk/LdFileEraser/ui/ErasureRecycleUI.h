/*
��������վ���ļ���
����վ��һ������Ŀ¼��ÿ���̷�����һ������վĿ¼
��ͬWindows����ϵͳ�Ļ���վĿ¼��ͬ��
ϵͳÿɾ��һ���ļ����ڻ���վ���������ļ���
һ����¼ԭ�ļ���Ϣ������Explorer��ʾ����һ������������ԭ���ļ���
������SHGetFileInfo�������ļ���Ӧ������


*/

#pragma once

#include "ErasureFileUI.h"

class CErasureRecycleUI : 
	public CErasureFileUI,
//	IEraserThreadCallback,  //�ļ������̻߳ص��������������״̬��������Ϣ��
	IGernalCallback<LPWIN32_FIND_DATA>,  //����վʵ���ļ�
	IGernalCallback<TCHAR*>              //ö�ٴ��̣�����·����
{
public:
	CErasureRecycleUI();
	virtual ~CErasureRecycleUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
//	CButtonUI* btnOk;
//	CFolderInfo m_RecycleFile;
//	CEreaserThrads m_EreaserThreads;  //��������߳�
//
//	typedef struct FILE_ERASURE_DATA
//	{
//		E_FILE_STATE nStatus;             //����״̬
//		DWORD        nErrorCode;          //������루�������
//		CControlUI* ui;                   //listView ��
//		DWORD nCount;                     //�ļ����µ��ܵ��ļ���
//		DWORD nErasued;                   //�Ѿ����������ļ���
//	}*PFILE_ERASURE_DATA;
//
//
//	void StatErase();
//	virtual void OnClick(TNotifyUI& msg);
//	void OnSelectChanged(TNotifyUI &msg) override;
//	void OnItemClick(TNotifyUI &msg) override;
	void EnumRecyleFiels();  
//	void FreeRecycleFiles(CLdArray<CVirtualFile*>* files);
//	DWORD SetFolderFilesData(CLdArray<CVirtualFile*>* files);
//	void DeleteErasuredFile(CLdArray<CVirtualFile*>* files);
//	bool OnListItemPaint(PVOID Param);

protected:
	void AttanchControl(CControlUI* pCtrl) override;
	//�����̻߳ص�����
//	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;
	//FindFirst ����վʵ���ļ�
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//FindFirstVolume ö�ٴ��̣�����·����
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;

};

