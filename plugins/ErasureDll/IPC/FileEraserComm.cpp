#include "stdafx.h"
#include "FileEraserComm.h"
#include "../../../LdApp/LdApp.h"

#define SHAREDATA_NAME _T("LD_FILE_ERASER_SHARE")
#define SHAREDATA_SIZE 1024

struct CALL_PARAM
{
	DWORD id;
	TCHAR Param_data_name[30]; 
};

CFileEraserComm::CFileEraserComm()
	: m_Data(nullptr)
{

}

CFileEraserComm::~CFileEraserComm()
{
	if (m_Data)
		delete m_Data;
}

DWORD CFileEraserComm::LoadHost(IGernalCallback<PVOID>* HostExit)
{
	DWORD result = ThisApp->RunPlugin(nullptr, RS_NONE, nullptr);
	//RunProcess();
	if(result)
	{
		CThread* thread = new CThread();
		
	}
	return result;
}

DWORD CFileEraserComm::ExecuteFileAnalysis(CLdArray<TCHAR*>* files)
{
	//m_Data->Write()
	Call(0, nullptr, 0, nullptr);
	return 0;
}

bool CFileEraserComm::Call(DWORD dwId, PVOID Param, WORD nParamSize, PVOID* result)
{
	CLdString data_name = NewGuidString();
	if (data_name.IsEmpty())
		return false;

	CShareData ParamData(data_name, nParamSize);
	ParamData.Write(Param, nParamSize);
	struct CALL_PARAM call_param = { 0 };
	call_param.id = dwId;
	data_name.CopyTo(call_param.Param_data_name);
	m_Data->Write(&call_param, sizeof(data_name));
	PVOID p = nullptr;
	WORD n;
	ParamData.Read(&p, &n);
	if (result)
		*result = p;
	else if (p)
		delete[] p;
	return true;
}
