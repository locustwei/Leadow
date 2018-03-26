#include "stdafx.h"
#include "Communication.h"

#define PIPE_NAME _T("8E557ACB-D1D3-4D30-989D-ECA43B8A9BDE")

CUIComm::CUIComm(ICommunicationListen* listen)
	:CLdCommunication(listen, PIPE_NAME)
{
}

CUIComm::~CUIComm()
{
}

DWORD CUIComm::Connect()
{
	return 0;
}

DWORD CUIComm::ExecuteFileAnalysis(CLdArray<CLdString>* files)
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

