#include "stdafx.h"
#include "Communication.h"
#include "../define.h"

#define PIPE_NAME _T("8E557ACB-D1D3-4D30-989D-ECA43B8A9BDE")

CFileEraserComm::CFileEraserComm()
	:CLdCommunication(this, PIPE_NAME)
{
}

CFileEraserComm::~CFileEraserComm()
{
}

DWORD CFileEraserComm::Connect()
{
	return 0;
}

DWORD CFileEraserComm::ExecuteFileAnalysis(CLdArray<CLdString>* files)
{
	DebugOutput(L"ExecuteFileAnalysis");

	if (!IsConnected())
		Connect();
	//m_Data->Write()
	CDynObject param;
	for(int i=0; i<files->GetCount(); i++)
	{
		param.AddArrayValue(EPN_FILES, *files[i]);
	}
	CLdString s = param.ToString();

	CallMethod(eci_anafiles, s.GetData(), (s.GetLength()+1) * sizeof(TCHAR), nullptr);

	return 0;
}

void CFileEraserComm::SendFileAnalyResult(TCHAR* FileName, PTEST_FILE_RESULT result)
{
	//todo 发送文件分析结果
}

bool CFileEraserComm::OnCreate()
{
	return true;
}

void CFileEraserComm::OnTerminate(DWORD exitcode)
{
}

void CFileEraserComm::OnCommand(WORD id, TCHAR* ProcessName, PVOID Param, WORD nParamSize)
{
	switch (id)
	{
	case eci_anafiles:
		DebugOutput(_T("OnCommand %s"), ProcessName);
	default:
		break;
	}
}

