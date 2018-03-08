#include "stdafx.h"
#include "FileEraserComm.h"
#include "../define.h"

#define PIPE_NAME _T("8E557ACB-D1D3-4D30-989D-ECA43B8A9BDE")

CFileEraserComm::CFileEraserComm(ICommunicationListen* listen)
	:CLdCommunication(listen, PIPE_NAME)
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
		param.AddArrayValue(EPN_ERASE_FILES, *files[i]);
	}
	CLdString s = param.ToString();

	CallMethod(eci_anafiles, s.GetData(), s.GetLength() * sizeof(TCHAR), nullptr);

	return 0;
}

void CFileEraserComm::AnalFile(TCHAR* file_name)
{

}
