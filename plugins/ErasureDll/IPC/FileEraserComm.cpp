#include "stdafx.h"
#include "FileEraserComm.h"
#include "../../../LdApp/LdApp.h"

#define SHAREDATA_NAME _T("LD_FILE_ERASER_SHARE")
#define SHAREDATA_SIZE 1024

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
