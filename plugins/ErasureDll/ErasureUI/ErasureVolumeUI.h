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
		E_FILE_STATE nStatus;             //擦除状态
		DWORD        nErrorCode;          //错误代码（如果错误）
		DWORD        FreespaceTime;       //擦除空闲空间所花的时间
		DWORD        TrackTime;           //擦除痕迹所花的的时间
		CControlUI* ui;                   //listView 行
	}*PFILE_ERASURE_DATA;

	CButtonUI* btnOk;  
	CLdArray<CVirtualFile*> m_Volumes;
	//CEreaserThrads m_EreaserThreads;  //管理擦除线程

	void StatAnalysis();  //磁盘数据分析
	void StatErase();     //磁盘擦除  
	void OnClick(TNotifyUI& msg);
	void UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui, int Percent, DWORD time);  //
	void ShowAnalysisResult(TCHAR*, CControlUI* ui); //分析数据在列表中显示
	bool OnAfterColumePaint(PVOID Param);
protected:
	void AttanchControl(CControlUI* pCtrl) override;
	BOOL EnumVolume_Callback(PVOID data, UINT_PTR Param);
	//擦除线程回掉，同步擦除状态
	bool EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue);
	bool GetViewHeader() override;
	bool AnalyResult(TCHAR* FileName, PVOID pData);
};

