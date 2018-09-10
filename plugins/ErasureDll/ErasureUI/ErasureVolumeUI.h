#pragma once
#include "ShFileView.h"
#include "../define.h"


class CErasureVolumeUI : 
	public CShFileViewUI
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
	//CEreaserThrads m_EreaserThreads;  //��������߳�

	void StatAnalysis();  //�������ݷ���
	void StatErase();     //���̲���  
	void OnClick(TNotifyUI& msg);
	void UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui, int Percent, DWORD time);  //
	void ShowAnalysisResult(TCHAR*, CControlUI* ui); //�����������б�����ʾ
	bool OnAfterColumePaint(PVOID Param);
protected:
	void AttanchControl(CControlUI* pCtrl) override;
	BOOL EnumVolume_Callback(PVOID data, UINT_PTR Param);
	//�����̻߳ص���ͬ������״̬
	bool EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue);
	bool GetViewHeader() override;
	bool AnalyResult(TCHAR* FileName, PVOID pData);
};

