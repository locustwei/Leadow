#pragma once
#include "../eraser/ErasureThread.h"
#include "ShFileView.h"

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
	CButtonUI* btnOk;
	CListUI* lstFile;
	CEreaserThrads m_EreaserThreads;
	CLdArray<CVirtualFile*> m_ErasureFiles;

	virtual void OnClick(DuiLib::TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
protected:
	void AttanchControl(CControlUI* pCtrl) override;
	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;

};

