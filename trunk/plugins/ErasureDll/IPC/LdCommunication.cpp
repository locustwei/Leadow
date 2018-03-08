#include "stdafx.h"
#include "LdCommunication.h"
#include <LdStructs.h>

CLdCommunication::CLdCommunication(ICommunicationListen* listen)
	: m_Data(nullptr)
	, m_hProcess(nullptr)
	, m_Connected(false)
{
	m_Listen = listen;
}

CLdCommunication::CLdCommunication(ICommunicationListen* listen, TCHAR* sharedata)
	: m_hProcess(nullptr)
	, m_Connected(true)
{
	m_Listen = listen;
	m_Data = new CShareData(sharedata);
	m_Data->StartReadThread(CMethodDelegate::MakeDelegate(this, &CLdCommunication::ShareData_Callback));
}

CLdCommunication::~CLdCommunication()
{
	TerminateHost();

	if (m_Data)
		delete m_Data;
}

DWORD CLdCommunication::TerminateHost()
{
//	if (!m_Data)
//		m_Data->Write();
	if (!m_hProcess)
	{
		TerminateProcess(m_hProcess, 0);
		CloseHandle(m_hProcess);
		m_hProcess = nullptr;
	}
	return 0;
}

DWORD CLdCommunication::LoadHost(TCHAR* plugid)
{

	if (!m_Data)
	{
		CLdString data_name = NewGuidString();
		if (data_name.IsEmpty())
			return false;
		m_Data = new CShareData(data_name);
		m_Data->StartReadThread(CMethodDelegate::MakeDelegate(this, &CLdCommunication::ShareData_Callback));
	}
	CLdString param;
	param.Format(_T("%s%s"), HOST_PARAM_PLUGID, plugid);

	DWORD result = ThisApp->RunPluginHost(param, false, &m_hProcess);
	if(result != 0)
	{
		m_Data->StopReadThread();
		delete m_Data;
		m_Data = nullptr;

		return result;
	}

	if(!m_Listen->OnCreate())
	{
		TerminateHost();
		m_Data->StopReadThread();
		delete m_Data;
		m_Data = nullptr;

		return result;
	}

	CThread* thread = new CThread();
	thread->Start(CMethodDelegate::MakeDelegate(this, &CLdCommunication::WaitHost), (UINT_PTR)m_hProcess);

	m_Connected = true;

	return result;
}

bool CLdCommunication::IsConnected()
{
	return m_Connected;
}

bool CLdCommunication::CallMethod(WORD dwId, PVOID Param, WORD nParamSize, PVOID* result, CMethodDelegate progress)
{

	DebugOutput(L"CallMethod id=%d, paramsize=%d", dwId, nParamSize);

	int len = sizeof(COMMUNICATE_DATA) + nParamSize;

	PCOMMUNICATE_DATA call_param = (PCOMMUNICATE_DATA)new BYTE[len];
	ZeroMemory(call_param, len);

	call_param->commid = dwId;
	call_param->nSize = nParamSize;
	if (Param && nParamSize)
		MoveMemory(&call_param->data, Param, nParamSize);

	if (!progress.IsNull())
	{
		CLdString data_name = NewGuidString();
		data_name.CopyTo(call_param->progress);

		CShareData* ParamData = new CShareData(data_name);
		ParamData->StartReadThread(progress, true);    //读数线程结束时对象自我销毁
	}

	//发送调用参数。
	m_Data->Write(call_param, len);
	
	delete[] (PBYTE)call_param;

	if (result)
	{
		PBYTE p = nullptr;
		WORD nSize = 0;
		if (m_Data->Read(&p, &nSize) == 0)
		{
			*result = p;
		}
	}
	return true;
}
//
INT_PTR CLdCommunication::ShareData_Callback(void* pData, UINT_PTR Param)
{
	PCOMMUNICATE_DATA  data = (PCOMMUNICATE_DATA)pData;
	
	DebugOutput(L"ShareData_Callback id=%d, paramsize=%d", data->commid, data->nSize);

	m_Listen->OnCommand(data->commid, data->data, data->nSize);
	return 1;
}

INT_PTR CLdCommunication::WaitHost(PVOID, UINT_PTR Param)
{
	HANDLE hProcess = (HANDLE)Param;
	WaitForSingleObject(hProcess, INFINITE);
	DWORD dwCode = 0;
	GetExitCodeProcess(hProcess, &dwCode);
	m_Listen->OnTerminate(dwCode);
	DebugOutput(L"WaitHost %d", dwCode);
	return 0;
}
