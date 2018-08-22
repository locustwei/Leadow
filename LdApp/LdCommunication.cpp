#include "stdafx.h"
#include "LdCommunication.h"
#include "LdStructs.h"
#include "LdApp.h"
#include "../plugins/plugin.h"
#include "LdPlugin.h"

CLdCommunication::CLdCommunication(ICommunicationListen* listen)
	: m_Data(nullptr)
	, m_hProcess(nullptr)
	, m_Connected(false)
{
	m_Listen = listen;
}

CLdCommunication::CLdCommunication(ICommunicationListen* listen, TCHAR* sharedata)
	: m_hProcess(nullptr)
	, m_Connected(false)
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

	DWORD result = 0xFFFFFFFF;

	do
	{

#ifdef _DEBUG
		CLdString s = ThisApp->GetAppPath();
		s += PLUGIN_PATH;
		CPluginManager pm(s);
		IPluginInterface* plug = pm.LoadPlugin(ThisApp, plugid);
		if (plug == nullptr)
			break;
		plug->InitCommunicate();
#else

		CLdString param;
		param.Format(_T("%s%s"), HOST_PARAM_PLUGID, plugid);

		result = ThisApp->RunPluginHost(param, false, &m_hProcess);
		if (result != 0)
			break;
#endif

		if (!m_Listen->OnCreate())
			break;

		CThread* thread = new CThread();
		thread->Start(CMethodDelegate::MakeDelegate(this, &CLdCommunication::WaitHost), (UINT_PTR)m_hProcess);
		result = 0;

	} while (false);

	if (result != 0)
	{
		TerminateHost();
		m_Data->StopReadThread();
		delete m_Data;
		m_Data = nullptr;
	}

	m_Connected = result == 0;
	return result;
}

bool CLdCommunication::IsConnected()
{
	return m_Connected;
}

bool CLdCommunication::CallMethod(WORD dwId, CDynObject& Param, CDynObject* result, ICommunicationListen* progress)
{

	CLdString tmp = Param.ToString();
	int len = sizeof(COMMUNICATE_DATA) + tmp.GetLength() * sizeof(TCHAR);

	PCOMMUNICATE_DATA call_param = (PCOMMUNICATE_DATA)new BYTE[len];
	ZeroMemory(call_param, len);

	call_param->commid = dwId;
	call_param->nSize = tmp.GetLength() * sizeof(TCHAR);
	if (call_param->nSize)
		MoveMemory(&call_param->data, tmp.GetData(), call_param->nSize);

	if (!progress)
	{
		CLdString data_name = NewGuidString();
		data_name.CopyTo(call_param->progress);

		CShareData* ParamData = new CShareData(data_name);
		ParamData->StartReadThread(progress, true);    //回掉函数progress返回0时线程结束，线程结束CShareData对象自我销毁
	}

	//发送调用参数。
	m_Data->Write(call_param, len);
	
	delete[] (PBYTE)call_param;

	if (result)
	{
		/*PBYTE p = nullptr;
		WORD nSize = 0;
		if (m_Data->Read(&p, &nSize) == 0)
		{
			*result = p;
		}*/
	}
	return true;
}
//
INT_PTR CLdCommunication::ShareData_Callback(void* pData, UINT_PTR Param)
{
	PCOMMUNICATE_DATA  data = (PCOMMUNICATE_DATA)pData;
	
	DebugOutput(L"ShareData_Callback id=%d, paramsize=%d\n", data->commid, data->nSize);

	DoRecvData(data);

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

void CLdCommunication::DoRecvData(PCOMMUNICATE_DATA data)
{
	if(m_Listen)
		m_Listen->OnCommand(data->commid, data->progress, data->data, data->nSize);
}
