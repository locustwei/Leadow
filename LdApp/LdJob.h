#pragma once

#define RUN_FLAG_ASADMINI 0x0010

class CLdJob
	: IThreadRunable
{
public:
	CLdJob();
	~CLdJob();

	DWORD RunProcess(TCHAR* cmd, TCHAR* param, DWORD dwFlag);
	VOID CreateShareData(TCHAR* pName);
protected:
	void ThreadBody(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
};

