#pragma once
#include "ShFileView.h"
#include "../define.h"
#include "LdCommunication.h"


class CErasureVolumeUI : 
	public CShFileViewUI
	, ICommunicationListen
{
public:
	CErasureVolumeUI();
	~CErasureVolumeUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	class CEraseVolumeData
	{
	public:
		CEraseVolumeData()
		{
			volume = nullptr;
			ui = nullptr;
		}
		~CEraseVolumeData()
		{
			if (volume)
				delete volume;
		}

	public:
		CLdString*   volume;
		E_FILE_STATE nStatus;             //����״̬
		DWORD        nErrorCode;          //������루�������
		DWORD        FreespaceTime;       //�������пռ�������ʱ��
		DWORD        TrackTime;           //�����ۼ������ĵ�ʱ��
		CControlUI* ui;                   //listView ��
	};

	CButtonUI* btnOk;  
	CLdArray<CEraseVolumeData*> m_Volumes;
	CLdCommunication* m_Comm;
	//CEreaserThrads m_EreaserThreads;  //��������߳�

	void StatAnalysis();  //�������ݷ���
	void StatErase();     //���̲���  
	void OnClick(TNotifyUI& msg);
	//void UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui, int Percent, DWORD time);  //
	void ShowAnalysisResult(TCHAR*, CControlUI* ui); //�����������б�����ʾ
	bool OnAfterColumePaint(PVOID Param);
	void AddDiskVolumes();
protected:
	void AttanchControl(CControlUI* pCtrl) override;
	BOOL EnumVolume_Callback(PVOID data, UINT_PTR Param);
	//�����̻߳ص���ͬ������״̬
	bool EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue);
	bool GetViewHeader() override;
	bool AnalyResult(TCHAR* FileName, PVOID pData);

	virtual bool OnCreate() override;
	virtual void OnTerminate(DWORD exitcode) override;
	virtual void OnCommand(WORD id, CDynObject& Param) override;

};

