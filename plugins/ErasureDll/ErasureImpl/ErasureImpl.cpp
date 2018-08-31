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
	, m_EraseThread()
	, m_Files()
	, m_Comm(nullptr)
{
	m_hModule = (HMODULE)ThisModule;
	m_EraseThread.SetCallback(this);
}

CErasureImpl::~CErasureImpl()
{
	FreeEraseFiles(&m_Files);
	if (m_Comm)
		delete m_Comm;

	ErasureImpl = nullptr;
}

void CErasureImpl::FreeEraseFiles(CLdArray<CVirtualFile*>* files)
{
	for (int i = 0; i<files->GetCount(); i++)
	{
		CVirtualFile * file = files->Get(i);
		if (file)
		{
			PFILE_ERASURE_DATA pData = (PFILE_ERASURE_DATA)file->GetTag();
			if (pData)
			{
				delete pData;
			}
			file->SetTag(0);
			if (file->GetFileType() == vft_folder)
				FreeEraseFiles(((CFolderInfo*)file)->GetFiles());
			//delete file;
		}
	}
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

//设置文件的目录指向，擦除时更新隶属文件夹的进度
DWORD CErasureImpl::SetFolderFilesData(CVirtualFile* pFile)
{
	DWORD nCount = 1;

	PFILE_ERASURE_DATA p = new FILE_ERASURE_DATA;
	ZeroMemory(p, sizeof(FILE_ERASURE_DATA));
	pFile->SetTag((UINT_PTR)p);
	if (pFile->GetFileType() == vft_folder)
	{
		//nCount = pFile->GetFiles()->GetCount();
		for (int i = 0; i < ((CFolderInfo*)pFile)->GetFiles()->GetCount(); i++)
		{
			CVirtualFile* file = ((CFolderInfo*)pFile)->GetFiles()->Get(i);
			nCount += SetFolderFilesData(file);
		}
	}

	p->nCount = nCount;

	return nCount;
}

DWORD CErasureImpl::EraseFiles(CDynObject& Param)
{

	CDynObject result;

	bool removedir = Param.GetBoolean(EPN_OP_REMOVEDIR);
	int k = Param.GetArrayCount(EPN_FILES);
	for(int i=0; i<k; i++)
	{
		CLdString s = Param.GetString(EPN_FILES, nullptr, i);
		if (s.IsEmpty())
			continue;

		CFileInfo* info;
		if (CFileUtils::IsDirectoryExists(s))
		{
			info = new CFolderInfo();
			info->SetFileName(s);
			((CFolderInfo*)info)->FindFiles(true);
		}
		else
		{
			info = new CFileInfo();
			info->SetFileName(s);
		}

		m_Files.Add(info);

		SetFolderFilesData(info);
	}

//	m_EraseThread.SetCallback(callback);
	m_EraseThread.GetOptions()->FileMothed = (ErasureMothedType)Param.GetInteger(EPN_OP_METHOD);
	m_EraseThread.GetOptions()->bRemoveFolder = Param.GetBoolean(EPN_UNDELFOLDER);
	m_EraseThread.SetEreaureFiles(&m_Files);
	m_EraseThread.StartEreasure(10);

	return 0;
}

DWORD CErasureImpl::EraseVolume(CDynObject& Param)
{
	int k = Param.GetArrayCount(EPN_FILES);
	for (int i = 0; i<k; i++)
	{
		CLdString s = Param.GetString(EPN_FILES, nullptr, i);
		if (s.IsEmpty())
			continue;

		CVolumeInfo* info = new CVolumeInfo();
		info->SetFileName(s);

	}

//	m_EraseThread.SetCallback(callback);
	m_EraseThread.GetOptions()->FileMothed = (ErasureMothedType)Param.GetInteger(EPN_OP_METHOD);
	m_EraseThread.GetOptions()->bRemoveFolder = Param.GetBoolean(EPN_UNDELFOLDER);
	m_EraseThread.SetEreaureFiles(&m_Files);
	m_EraseThread.StartEreasure(k);

	return 0;

}

typedef struct ERASE_FILE_PARAM {
	CLdString progress;
	
	BOOL bRemoveFolder;
}*PERASE_FILE_PARAM;


DWORD CErasureImpl::FileAnalysis(CDynObject& Param)
{
	DebugOutput(L"FileAnalysis");

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

	m_Comm->CallMethod(eci_anafiles, result);

	return 0;
}

DWORD CErasureImpl::AnaVolume(CDynObject& Param)
{
	int k = Param.GetArrayCount(EPN_FILES);
	for (int i = 0; i<k; i++)
	{
		CLdString s = Param.GetString(EPN_FILES, nullptr, i);
		if (s.IsEmpty())
			continue;

		CVolumeInfo* info = new CVolumeInfo();
		info->SetFileName(s);
		m_Files.Add(info);
	}

//	m_EraseThread.SetCallback(callback);
	m_EraseThread.SetEreaureFiles(&m_Files);
	m_EraseThread.StartAnalysis(k);

	return 0;
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
	case eci_erasefiles:
		EraseFiles(Param);
		break;
	default:
		DebugOutput(L"unknown command id=%d", id);
	}
}
