#include "stdafx.h"
#include "LdJob.h"


CLdJob::CLdJob()
{
}


CLdJob::~CLdJob()
{
}

DWORD CLdJob::RunProcess(TCHAR* cmd, TCHAR* param, DWORD dwFlag)
{
	CLdString invoker = cmd;

	if (dwFlag & RUN_FLAG_ASADMINI)
	{
		SHELLEXECUTEINFO info = { 0 };
		info.cbSize = sizeof(SHELLEXECUTEINFO);
		info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_DEFAULT;
		info.lpVerb = TEXT("runas");
		info.lpFile = invoker;
		info.lpParameters = param;

		if (!ShellExecuteEx(&info))
			return 0;
		CThread* thread = new CThread(this);
		thread->Start((UINT_PTR)info.hProcess);
	}
	else
	{
		PROCESS_INFORMATION info = { 0 };
		STARTUPINFO si = { 0 };
		si.cb = sizeof(STARTUPINFO);
		if (CreateProcess(invoker, param, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &info))
			return 0;
	}
	return 1;
}

void CLdJob::CreateShareData(TCHAR* pName)
{
	CShareData* data = new CShareData(pName, 1024);
}

void CLdJob::ThreadBody(CThread* Sender, UINT_PTR Param)
{
	HANDLE hProcess = (HANDLE)Param;
	DWORD k = WaitForSingleObject(hProcess, INFINITE);
	DWORD c;
	GetExitCodeProcess(hProcess, &c);
}

void CLdJob::OnThreadInit(CThread* Sender, UINT_PTR Param)
{
}

void CLdJob::OnThreadTerminated(CThread* Sender, UINT_PTR Param)
{
}
