#include "stdafx.h"
#include "FileEraserComm.h"
#include "../../../LdApp/LdApp.h"
#include "../../../LdApp/LdStructs.h"

#define SHAREDATA_NAME _T("LD_FILE_ERASER_SHARE")
#define SHAREDATA_SIZE 1024

struct CALL_PARAM
{
	DWORD id;
	TCHAR Param_data_name[30]; 
};

CFileEraserComm::CFileEraserComm()
	: m_Data(nullptr)
	, m_hProcess(nullptr)
{

}

CFileEraserComm::~CFileEraserComm()
{
	if (m_Data)
		delete m_Data;
}

DWORD CFileEraserComm::TerminateHost()
{
	if (!m_hProcess)
		return (DWORD)-1;
	if(!m_Data)
		TerminateProcess(m_hProcess, 0);
	CloseHandle(m_hProcess);
	m_hProcess = nullptr;
	return 0;
}

DWORD CFileEraserComm::LoadHost(CMethodDelegate OnCreated, CMethodDelegate OnDestroy)
{
	CLdString data_name = NewGuidString();
	if (data_name.IsEmpty())
		return false;
	//m_Data = new CShareData(data_name, 1024);
	data_name.Insert(0, HOST_PARAM_PIPENAME);
	//HANDLE hProcess = nullptr;
	DWORD result = ThisApp->RunPluginHost(data_name, false, &m_hProcess);
	if(result != 0)
	{
		//delete m_Data;
		return result;
	}
	if(!OnCreated(0))
	{
		//delete m_Data;
		TerminateHost();
		return result;
	}

	if(result)
	{
		CThread* thread = new CThread();
		thread->Start(CMethodDelegate::MakeDelegate(this, CFileEraserComm::WaitHost), (HANDLE)m_hProcess);
	}
	m_Data->StartReadThread(this);
	return result;
}

DWORD CFileEraserComm::ExecuteFileAnalysis(CLdArray<TCHAR*>* files)
{
	//m_Data->Write()
	CallMethod(0, nullptr, 0, nullptr);
	return 0;
}

bool CFileEraserComm::CallMethod(DWORD dwId, PVOID Param, WORD nParamSize, PVOID* result, IGernalCallback<PVOID>* progress)
{
	//生产GUID作为共享内存的名字
	CLdString data_name = NewGuidString();
	if (data_name.IsEmpty())
		return false;
	CShareData* ParamData = new CShareData(data_name, nParamSize);
	ParamData->Write(Param, nParamSize);
	struct CALL_PARAM call_param = { 0 };
	call_param.id = dwId;
	data_name.CopyTo(call_param.Param_data_name);
	//发送调用参数。
	m_Data->Write(&call_param, sizeof(data_name));
	PBYTE p = nullptr;
	WORD n;
	//等待返回
	ParamData->Read(&p, &n);
	if (progress)
		ParamData->StartReadThread(progress, true);
	if (result)
		*result = p;
	else if (p)
		delete[] p;
	if(!progress)
		delete ParamData;
	return true;
}

BOOL CFileEraserComm::GernalCallback_Callback(void* pData, UINT_PTR Param)
{
	return true;
}

void CFileEraserComm::WaitHost(UINT_PTR Param)
{
	HANDLE hProcess = (HANDLE)Param;
	WaitForSingleObject(hProcess, INFINITE);
}
