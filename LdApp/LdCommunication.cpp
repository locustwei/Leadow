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
	, m_ResultObj(nullptr)
{
	m_Listener = listen;
	m_hWaitResult = CreateEvent(nullptr, FALSE, TRUE, nullptr);
	if (m_hWaitResult != INVALID_HANDLE_VALUE)
		ResetEvent(m_hWaitResult);
}

CLdCommunication::CLdCommunication(ICommunicationListen* listen, TCHAR* sharedata)
	: m_hProcess(nullptr)
	, m_Connected(false)
	, m_ResultObj(nullptr)
{
	m_Listener = listen;
	m_hWaitResult = CreateEvent(nullptr, FALSE, TRUE, nullptr);
	if (m_hWaitResult != INVALID_HANDLE_VALUE)
		ResetEvent(m_hWaitResult);
	m_Data = new CShareData(sharedata);
	m_Data->StartReadThread(CLdMethodDelegate::MakeDelegate(this, &CLdCommunication::ShareData_Callback), 0);
}

CLdCommunication::~CLdCommunication()
{
	if (m_hWaitResult != INVALID_HANDLE_VALUE)
		SetEvent(m_hWaitResult);

	TerminateHost();

	if (m_Data)
		delete m_Data;
	CloseHandle(m_hWaitResult);
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
		m_Data->StartReadThread(CLdMethodDelegate::MakeDelegate(this, &CLdCommunication::ShareData_Callback), 0);
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
#else

		CLdString param;
		param.Format(_T("%s%s"), HOST_PARAM_PLUGID, plugid);

		result = ThisApp->RunPluginHost(param, false, &m_hProcess);
		if (result != 0)
			break;
#endif

		if (!m_Listener->OnCreate())
			break;

		CThread* thread = new CThread();
		thread->Start(CLdMethodDelegate::MakeDelegate(this, &CLdCommunication::WaitHost), (UINT_PTR)m_hProcess);
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
	int len = sizeof(COMMUNICATE_DATA) + (tmp.GetLength()+1) * sizeof(TCHAR);

	PCOMMUNICATE_DATA call_param = (PCOMMUNICATE_DATA)new BYTE[len];
	ZeroMemory(call_param, len);

	call_param->commid = dwId;
	call_param->nSize = (tmp.GetLength()+1) * sizeof(TCHAR);
	if (!tmp.IsEmpty())
		tmp.CopyTo((TCHAR*)call_param->data);

	if (progress)
	{
		CLdString data_name = NewGuidString();
		data_name.CopyTo(call_param->progress);

		CShareData* ParamData = new CShareData(data_name);
		ParamData->StartReadThread(CLdMethodDelegate::MakeDelegate(this, &CLdCommunication::ShareData_Callback), (UINT_PTR)progress, true);    //回掉函数progress返回0时线程结束，线程结束CShareData对象自我销毁
	}

	//发送调用参数。
	DebugOutput(L"CallMethod %x %s\n", this, Param.ToString());
	m_Data->Write(call_param, len);
	
	delete[] (PBYTE)call_param;

	if (result)
	{
		m_ResultObj = result;
		WaitForSingleObject(m_hWaitResult, INFINITE);
		DebugOutput(L"CallMethod return %d\n", dwId);
	}
	return true;
}

bool CLdCommunication::SendResult(WORD dwId, CDynObject& Param)
{
	CLdString tmp = Param.ToString();
	int len = sizeof(COMMUNICATE_DATA) + (tmp.GetLength() + 1) * sizeof(TCHAR);

	PCOMMUNICATE_DATA call_param = (PCOMMUNICATE_DATA)new BYTE[len];
	ZeroMemory(call_param, len);

	call_param->commid = dwId;
	call_param->refer = dwId;
	call_param->nSize = (tmp.GetLength() + 1) * sizeof(TCHAR);
	if (!tmp.IsEmpty())
		tmp.CopyTo((TCHAR*)call_param->data);

	m_Data->Write(call_param, len);

	delete[](PBYTE)call_param;

	return true;
}

/************************************************************************/
/* 接收到消息在主线程调用*/
/************************************************************************/
BOOL CLdCommunication::RunOnMainThread(PVOID, UINT_PTR Param)
{
	PCALLBACK_DATA data = (PCALLBACK_DATA)Param;

	CDynObject param = (TCHAR*)data->commdata.data;

	data->listener->OnCommand(data->commdata.commid, param);

	delete[] data;

	return TRUE;
}

void CLdCommunication::DoResultData(PCOMMUNICATE_DATA data)
{
	if (m_ResultObj)
		m_ResultObj->PrepareStr((TCHAR*)data->data);
	SetEvent(m_hWaitResult);
}

//
INT_PTR CLdCommunication::ShareData_Callback(void* pData, UINT_PTR Param)
{
	PCOMMUNICATE_DATA  data = (PCOMMUNICATE_DATA)pData;
	
	DebugOutput(L"ShareData_Callback %x, param=%s\n", this, (TCHAR*)data->data);

	if (data->refer != 0)
	{
		DoResultData(data);
	}else if (Param == 0)
		DoRecvData(data, m_Listener);
	else
		DoRecvData(data, (ICommunicationListen*)Param);

	return 1;
}

//监视进程结束。
INT_PTR CLdCommunication::WaitHost(PVOID, UINT_PTR Param)
{  
	HANDLE hProcess = (HANDLE)Param;
	WaitForSingleObject(hProcess, INFINITE);
	DWORD dwCode = 0;
	GetExitCodeProcess(hProcess, &dwCode);
	m_Listener->OnTerminate(dwCode);
	DebugOutput(L"WaitHost %d", dwCode);
	return 0;
}

void CLdCommunication::DoRecvData(PCOMMUNICATE_DATA data, ICommunicationListen* listener)
{
	if (listener)
	{	//发到主线程调用OnCommand
		PCALLBACK_DATA calldata = (PCALLBACK_DATA)new BYTE[sizeof(CALLBACK_DATA)+data->nSize];
		calldata->listener = listener;
		MoveMemory(&calldata->commdata, data, sizeof(COMMUNICATE_DATA) + data->nSize);
#ifdef _DEBUG
		RunOnMainThread(nullptr, (UINT_PTR)calldata);
#else
		ThisApp->Send2MainThread(this, (UINT_PTR)calldata);
#endif
	}
}
