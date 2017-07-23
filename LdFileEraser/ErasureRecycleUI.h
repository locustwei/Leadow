/*
��������վ���ļ���
����վ��һ������Ŀ¼��ÿ���̷�����һ������վĿ¼
��ͬWindows����ϵͳ�Ļ���վĿ¼��ͬ��
ϵͳÿɾ��һ���ļ����ڻ���վ���������ļ���
һ����¼ԭ�ļ���Ϣ������Explorer��ʾ����һ������������ԭ���ļ���
������SHGetFileInfo�������ļ���Ӧ������

*/

#pragma once

#include "ErasureThread.h"
#include "ShFileView.h"

class LDLIB_API CErasureRecycleUI : 
	public CShFileViewUI,
	//public CFramNotifyPump, 
	IFindCompare<PERASURE_FILE_DATA>,    //�ļ������Ƚϻص�
	ISortCompare<PERASURE_FILE_DATA>,    //�ļ�������ȽϺ�����
	IGernalCallback<PERASE_CALLBACK_PARAM>,  //�ļ������̻߳ص��������������״̬��������Ϣ��
	IGernalCallback<LPWIN32_FIND_DATA>,  //����վʵ���ļ�
	IGernalCallback<TCHAR*>              //ö�ٴ��̣�����·����
{
public:
	CErasureRecycleUI();
	virtual ~CErasureRecycleUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOk;
	CTreeNodes<PERASURE_FILE_DATA> m_RecycleFiles;//����վ�е�ʵ���ļ�����ʾ��UI�еĲ��������ļ���
	CEreaserThrads m_EreaserThreads;  //��������߳�

	void StatErase();
	virtual void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI &msg) override;
	void OnItemClick(TNotifyUI &msg) override;
	void EnumRecyleFiels();  
	void FreeRecycleFiles(CTreeNodes<PERASURE_FILE_DATA>* files);
	DWORD SetFolderFilesData(PERASURE_FILE_DATA pFolder, CTreeNodes<ERASURE_FILE_DATA*>* files);
protected:
	void AttanchControl(CControlUI* pCtrl) override;

	BOOL GernalCallback_Callback(PERASE_CALLBACK_PARAM pData, UINT_PTR Param) override;

	//FindFirst ����վʵ���ļ�
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//FindFirstVolume ö�ٴ��̣�����·����
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;
	//����ӿ�
	int ArraySortCompare(ERASURE_FILE_DATA** it1, ERASURE_FILE_DATA** it2) override;
	int ArrayFindCompare(PVOID key, ERASURE_FILE_DATA** it) override;

};

