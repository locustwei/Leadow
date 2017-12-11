#pragma once
#include "EraserCComm.h"

class CEraserListenImpl:
	public IEraserListen
	,public CEraserCComm
{
public:
	CEraserListenImpl();
	~CEraserListenImpl();
	
	bool AnalyResult(TCHAR* FileName, PVOID pData) override;
protected:
	bool EraserReprotStatus(TCHAR* pFile, E_THREAD_OPTION op, DWORD dwValue) override;

	void OnClosed(CLdSocket*) override;
	void OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength) override;
	void SetSocket(CLdSocket*) override;
	void SetContext(PVOID) override;
private:
	bool m_Abort;
};

