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

DWORD CErasureImpl::EraseFile(CDynObject& Param)
{
	//int mothed = Param.GetInteger(EPN_MOTHED, 3);
	//BOOL removefolder = Param.GetBoolean(EPN_UNDELFOLDER, true);
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
	m_EraseThread.GetOptions()->FileMothed = (ErasureMothedType)Param.GetInteger(EPN_MOTHED);
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
	m_EraseThread.GetOptions()->FileMothed = (ErasureMothedType)Param.GetInteger(EPN_MOTHED);
	m_EraseThread.GetOptions()->bRemoveFolder = Param.GetBoolean(EPN_UNDELFOLDER);
	m_EraseThread.SetEreaureFiles(&m_Files);
	m_EraseThread.StartEreasure(k);

	return 0;

}

typedef struct ERASE_FILE_PARAM {
	CLdString progress;
	
	BOOL bRemoveFolder;
}*PERASE_FILE_PARAM;


DWORD CErasureImpl::FileAnalysis(TCHAR* Param, TCHAR* progress)
{
	DebugOutput(L"FileAnalysis");

	//bool undelfolder = Param.GetBoolean(EPN_UNDELFOLDER);

	//int k = Param.GetArrayCount(EPN_FILES);

	//for (int i = 0; i<k; i++)
	//{
	//	CLdString s = Param.GetString(EPN_FILES, nullptr, i);
	//	if (s.IsEmpty())
	//		continue;

	//	DebugOutput(L"FileAnalysis %s", s.GetData());

	//	PERASE_FILE_PARAM Param = new ERASE_FILE_PARAM;
	//	Param->FileName = s;
	//	Param->method = new CErasureMothed(em_Pseudorandom);
	//	Param->bRemoveFolder = undelfolder;

	//	CThread* thread = new CThread();
	//	thread->Start(CMethodDelegate::MakeDelegate(this, &CErasureImpl::FileAnalyThread), (UINT_PTR)Param);
	//}

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

INT_PTR CErasureImpl::FileAnalyThread(PVOID pData, UINT_PTR Param)
{
	//PERASE_FILE_PARAM pParam = (PERASE_FILE_PARAM)Param;
	//CEraseTest test;
	//TEST_FILE_RESULT result = test.TestFile(pParam->FileName, pParam->bRemoveFolder);
	//delete pParam;
	//m_Comm->SendFileAnalyResult(pParam->FileName, &result);
	return 0;
}

CFramNotifyPump* CErasureImpl::CreateUI()
{
	return nullptr;
}

DWORD CErasureImpl::InitCommunicate()
{
	DebugOutput(L"InitCommunicate");

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

void CErasureImpl::OnCommand(WORD id, TCHAR* ProcessName, PVOID data, WORD nSize)
{
	
	switch(id)
	{
	case eci_anafiles:
		FileAnalysis((TCHAR*)data, ProcessName);
		break;
	default:
		DebugOutput(L"unknown command id=%d", id);
	}
}

//DWORD CFileEraserComm::ExecuteFileAnalysis(CLdArray<CLdString>* files)
//{
//	DebugOutput(L"ExecuteFileAnalysis");
//
//	if (!IsConnected())
//		Connect();
//	//m_Data->Write()
//	CDynObject param;
//	for (int i = 0; i < files->GetCount(); i++)
//	{
//		param.AddArrayValue(EPN_FILES, *files[i]);
//	}
//	CLdString s = param.ToString();
//
//	CallMethod(eci_anafiles, s.GetData(), (s.GetLength() + 1) * sizeof(TCHAR), nullptr);
//
//	return 0;
//}
