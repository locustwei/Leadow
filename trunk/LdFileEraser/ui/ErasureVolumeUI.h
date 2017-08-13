#pragma once
#include "../eraser/ErasureThread.h"
#include "ShFileView.h"


class CErasureVolumeUI : 
	public CShFileViewUI,
	IEraserThreadCallback,  //�ļ������̻߳ص��������������״̬��������Ϣ��
	IGernalCallback<TCHAR*>              //ö�ٴ��̣�����·����
{
public:
	CErasureVolumeUI();
	virtual ~CErasureVolumeUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	typedef struct FILE_ERASURE_DATA
	{
		E_FILE_STATE nStatus;             //����״̬
		DWORD        nErrorCode;          //������루�������
		DWORD        FreespaceTime;       //�������пռ�������ʱ��
		DWORD        TrackTime;           //�����ۼ������ĵ�ʱ��
		CControlUI* ui;                   //listView ��
	}*PFILE_ERASURE_DATA;

	CButtonUI* btnOk;  
	CLdArray<CVirtualFile*> m_Volumes;
	CEreaserThrads m_EreaserThreads;  //��������߳�

	void StatAnalysis();  //�������ݷ���
	void StatErase();     //���̲���  
	void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI &msg) override;
	void OnItemClick(TNotifyUI &msg) override;
	void UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui);  //
	void ShowAnalysisResult(CVolumeEx* pVolume, CControlUI* ui); //�����������б�����ʾ
	bool OnAfterColumePaint(PVOID Param);
protected:
	void AttanchControl(CControlUI* pCtrl) override;
	//FindFirstVolume ö�ٴ��̣�����·����
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;
	//�����̻߳ص���ͬ������״̬
	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;
};

