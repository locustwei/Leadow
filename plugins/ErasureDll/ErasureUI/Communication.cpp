#include "stdafx.h"
#include "Communication.h"

#define PIPE_NAME _T("8E557ACB-D1D3-4D30-989D-ECA43B8A9BDE")
#define IMPL_PLUGIN_ID _T("BCBE2CB1-37FC-46C2-A9A2-9B9EEBEC262E")


CUIComm::CUIComm()
	:CLdCommunication(this, PIPE_NAME)
{
}

CUIComm::~CUIComm()
{
}

DWORD CUIComm::Connect()
{
	return LoadHost(IMPL_PLUGIN_ID);
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


bool CUIComm::OnCreate()
{
	return true;
}

void CUIComm::OnTerminate(DWORD exitcode)
{
}

void CUIComm::OnCommand(WORD id, TCHAR* ProcessName, PVOID Param, WORD nParamSize)
{
	switch (id)
	{
	case eci_anafiles:
		DebugOutput(_T("OnCommand %s"), ProcessName);
		break;
	default:
		break;
	}
}

