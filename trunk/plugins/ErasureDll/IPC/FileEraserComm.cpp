#include "stdafx.h"
#include "FileEraserComm.h"

struct CALL_PARAM
{
	DWORD id;
	TCHAR Param_data_name[30]; 
};

CFileEraserComm::CFileEraserComm(ICommunicationListen* listen)
	:CLdCommunication(listen)
{
}

CFileEraserComm::CFileEraserComm(ICommunicationListen* listen, TCHAR* sharedata)
	:CLdCommunication(listen, sharedata)
{
}

CFileEraserComm::~CFileEraserComm()
{
}

DWORD CFileEraserComm::LoadHost()
{
	DWORD result = CLdCommunication::LoadHost();

	return result;
}

DWORD CFileEraserComm::ExecuteFileAnalysis(CLdArray<TCHAR*>* files)
{
	//m_Data->Write()
	CallMethod(0, nullptr, 0, nullptr);
	return 0;
}

BOOL CFileEraserComm::GernalCallback_Callback(void* pData, UINT_PTR Param)
{
	return CLdCommunication::GernalCallback_Callback(pData, Param);
}
