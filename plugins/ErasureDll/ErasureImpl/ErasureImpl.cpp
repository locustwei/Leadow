#include "stdafx.h"
#include "../../plugin.h"
#include "ErasureThread.h"
#include "../define.h"
#include "ErasureImpl.h"
#include "EraseTest.h"

#define PLUGIN_ID _T("BCBE2CB1-37FC-46C2-A9A2-9B9EEBEC262E")
#define PIPE_NAME _T("8E557ACB-D1D3-4D30-989D-ECA43B8A9BDE")

CErasureImpl* ErasureImpl = nullptr;

#pragma region 动态库导出函数

#define CreatePluginImpl() new CErasureImpl();
#define DeletePluginImpl() \
if(ErasureImpl)            \
{                          \
	delete ErasureImpl;    \
	ErasureImpl = nullptr; \
}                          \

PLUGIN_PROPERTY GetSelfDesc()
{
	PLUGIN_PROPERTY ErasurePrpperty = { PLUGIN_USAGE_BK, PLUGIN_ID };
	return ErasurePrpperty;
}

API_Init();
API_UnInit();
API_Register();

#pragma endregion  动态库导出函数

CErasureImpl::CErasureImpl()
	: m_hModule(nullptr)
	, m_Comm(nullptr)
{
	m_hModule = (HMODULE)ThisModule;
}

CErasureImpl::~CErasureImpl()
{
	if (m_Comm)
		delete m_Comm;

	ErasureImpl = nullptr;
}

HMODULE CErasureImpl::GetModuleHandle()
{
	return m_hModule;
}

bool CErasureImpl::EraserReprotStatus(TCHAR* FileName, TCHAR* subfile, E_THREAD_OPTION op, DWORD dwValue)
{
	CDynObject param;
	if(FileName)
		param.AddObjectAttribute(_T("name"), FileName);
	if(subfile)
		param.AddObjectAttribute(_T("subfile"), subfile);
	param.AddObjectAttribute(_T("op"), op);
	param.AddObjectAttribute(_T("value"), (UINT)dwValue);
	m_Comm->CallMethod(eci_filestatus, param);
	return true;
}

bool CErasureImpl::AnalyResult(TCHAR* FileName, PVOID pData)
{
	return true;
}

DWORD CErasureImpl::EraseFiles(CDynObject& Param)
{

	CDynObject result;
	CLdArray<TCHAR*> Files;

	bool removedir = Param.GetBoolean(EPN_OP_REMOVEDIR);
	int k = Param.GetArrayCount(EPN_FILES);
	for(int i=0; i<k; i++)
	{
		CLdString s = Param.GetString(EPN_FILES, nullptr, i);
		if (s.IsEmpty())
			continue;
		Files.Add(s);

	}
	CEreaserThrads* EraseThread = new CEreaserThrads();
	EraseThread->SetCallback(this);
	EraseThread->GetOptions()->FileMothed = (ErasureMothedType)Param.GetInteger(EPN_OP_METHOD);
	EraseThread->GetOptions()->bRemoveFolder = Param.GetBoolean(EPN_UNDELFOLDER);
	return EraseThread->StartEraseFiles(&Files, 10);
}

DWORD CErasureImpl::EraseVolume(CDynObject& Param)
{
	int k = Param.GetArrayCount(EPN_FILES);
	for (int i = 0; i<k; i++)
	{
		CLdString s = Param.GetString(EPN_FILES, nullptr, i);
		if (s.IsEmpty())
			continue;

		CVolumeInfo* info = new CVolumeInfo(nullptr);

	}

	return 0;

}

DWORD CErasureImpl::FileAnalysis(CDynObject& Param)
{

	CDynObject result;

	bool undelfolder = Param.GetBoolean(EPN_UNDELFOLDER);

	int k = Param.GetArrayCount(EPN_FILES);
	CEraseTest test;
	TEST_FILE_RESULT tr;
	bool removedir = Param.GetBoolean(EPN_OP_REMOVEDIR);
	for (int i = 0; i<k; i++)
	{
		ZeroMemory(&tr, sizeof(TEST_FILE_RESULT));
		DWORD error = 0;
		CLdString s = Param.GetString(EPN_FILES, nullptr, i);
		
		DebugOutput(L"FileAnalysis %s\n", s.GetData());
	
		if (s.IsEmpty())
		{
			error = (DWORD)-1;
			continue;
		}
		else
		{
			error = test.TestFile(s, true, &tr);
		}

		CDynObject obj;
		obj.AddObjectAttribute(_T("name"), s);

		obj.AddObjectAttribute(EPN_ERROR_CODE, (int)error);
		if (error == 0)
		{
			if(tr.ADSCount)
				obj.AddObjectAttribute(_T("ADSCount"), tr.ADSCount);
			if(tr.ADSSizie)
				obj.AddObjectAttribute(_T("ADSSizie"), (int64_t)tr.ADSSizie);
			if(tr.FileCount)
				obj.AddObjectAttribute(_T("FileCount"), tr.FileCount);
			if(tr.TotalSize)
				obj.AddObjectAttribute(_T("TotalSize"), (int64_t)tr.TotalSize);
		}
		result.AddArrayValue(EPN_FILES, obj);
	}

	m_Comm->SendResult(eci_anafiles, result);

	return 0;
}

DWORD CErasureImpl::VolumeAnalysis(CDynObject& Param)
{
	CLdArray<TCHAR*> volumes;

	int k = Param.GetArrayCount(EPN_FILES);
	for (int i = 0; i<k; i++)
	{
		CLdString s = Param.GetString(EPN_FILES, nullptr, i);
		if (s.IsEmpty())
			continue;

		volumes.Add(s);
	}

	CEreaserThrads* EraseThread = new CEreaserThrads();
	EraseThread->SetCallback(this);
	EraseThread->GetOptions()->FileMothed = (ErasureMothedType)Param.GetInteger(EPN_OP_METHOD);
	EraseThread->GetOptions()->bRemoveFolder = Param.GetBoolean(EPN_UNDELFOLDER);
	return EraseThread->StartVolumeAnalysis(&volumes, volumes.GetCount());
}

CFramNotifyPump* CErasureImpl::CreateUI()
{
	return nullptr;
}

DWORD CErasureImpl::InitCommunicate()
{
	DebugOutput(L"InitCommunicate\n");

	m_Comm = new CLdCommunication(this, PIPE_NAME);
	return 0;
}

bool CErasureImpl::OnCreate()
{
	return true;
}

void CErasureImpl::OnTerminate(DWORD exitcode)
{
}

void CErasureImpl::OnCommand(WORD id, CDynObject& Param)
{
	
	switch(id)
	{
	case eci_anafiles:
		FileAnalysis(Param);
		break;
	case eci_anavolume:
		VolumeAnalysis(Param);
		break;
	case eci_erasefiles:
		EraseFiles(Param);
		break;
	default:
		DebugOutput(L"unknown command id=%d", id);
	}
}
