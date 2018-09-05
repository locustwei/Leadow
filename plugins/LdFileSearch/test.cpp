// test.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <iostream>
#include <Wincrypt.h>
#include <SetupAPI.h>
#include <vector>
#include "LdString.h"
#include "WJDefines.h"
#include "ExportFunction.h"

#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")

using namespace  std;
using namespace LeadowLib;

bool is_vista_or_later()
{
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

	if (dwVersion < 0x80000000)	// Windows NT/2000, Whistler
	{
		if(dwWindowsMajorVersion >= 6)	// Windows vista or later
		{
			return true;
		}
	}
	return false;
}

BOOL IsElevated( BOOL * pbElevated ) /*另一种判断是否高低权限的办法*/
{
	if( !is_vista_or_later() )
	{
		if ( pbElevated)
			*pbElevated = TRUE;
		return TRUE ;
	}
	HRESULT hResult = E_FAIL; // assume an error occured
	HANDLE hToken	= NULL;
	BOOL bRet = FALSE;
	if ( !::OpenProcessToken( 
		::GetCurrentProcess(), 
		TOKEN_QUERY, 
		&hToken ) )
	{
		return FALSE ;
	}

	TOKEN_ELEVATION te = { 0 };
	DWORD dwReturnLength = 0;

	if ( !::GetTokenInformation(
		hToken,
		TokenElevation,
		&te,
		sizeof( te ),
		&dwReturnLength ) )
	{
		DebugBreak();
	}
	else
	{
		if (dwReturnLength != sizeof( te ))
		{
			DebugBreak();
		}

		bRet = TRUE ; 

		if ( pbElevated)
			*pbElevated = (te.TokenIsElevated != 0);
	}

	::CloseHandle( hToken );

	return bRet;
}

#define SEE_MASK_FORCENOIDLIST 0x1000

void WipeShellExecuteEx(LPCTSTR lpFile, LPCTSTR pCommandLine, int nShow,  DWORD dwWaitInputIdle = 0, HANDLE* lphProcess = NULL, bool b_need_admin = false)
{
	SHELLEXECUTEINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.lpFile = lpFile;
	si.lpParameters = pCommandLine;
	si.nShow = nShow;
	si.fMask = (SEE_MASK_FLAG_NO_UI | SEE_MASK_FORCENOIDLIST);
	si.lpVerb = L"open";
	if (is_vista_or_later() && b_need_admin)
	{
		si.lpVerb = L"runas";
	}

	if (dwWaitInputIdle || lphProcess)
		si.fMask |= SEE_MASK_NOCLOSEPROCESS;

	::ShellExecuteEx(&si);

	if (dwWaitInputIdle)
		WaitForInputIdle(si.hProcess, dwWaitInputIdle);

	if(lphProcess)
		*lphProcess = si.hProcess;
	else
		CloseHandle(si.hProcess);
}

bool get_app_path(std::wstring& wstr_app_path, HMODULE hm_handle = NULL)
{
	wchar_t wsz_temp[_MAX_PATH] = {0};
	if (0 == ::GetModuleFileNameW(hm_handle, wsz_temp, _MAX_PATH))
	{
		return false;
	}
	wstr_app_path = wsz_temp;
	std::wstring::size_type pos = 0;
	pos = wstr_app_path.find_last_of(L'\\');
	if (std::wstring::npos == pos)
	{
		return false;
	}
	wstr_app_path.erase(pos);
	wstr_app_path += L"\\";
	return true;
}

LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* _pExcp)
{
	TCHAR path[MAX_PATH] = { 0 };
	GetModuleFileName(nullptr, path, MAX_PATH);
	CLdString s = path;
	s += L".dmp";

	HANDLE hFile = CreateFile(s, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
			hFile = CreateFile(s, GENERIC_WRITE | GENERIC_READ, 0, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	MINIDUMP_EXCEPTION_INFORMATION einfo = { 0 };
	einfo.ThreadId = ::GetCurrentThreadId();
	einfo.ExceptionPointers = _pExcp;
	einfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		MiniDumpWithCodeSegs,  // 指定生成默认的Minidump文件
		&einfo,
		NULL,
		NULL);
	CloseHandle(hFile);

	Sleep(10000);

	return   EXCEPTION_CONTINUE_SEARCH;
}


IWJLibInterface* g_lib = nullptr;

class CHandlerImpl : 
	//CIntefaceImpl,
	public IWJSHandler
{
public:
	CHandlerImpl()
	{
		//indexfile = nullptr;
	};

	~CHandlerImpl() 
	{
		//if (indexfile)
			//indexfile->Close();
	};

	virtual VOID  OnBegin(PVOID) override
	{
		t = GetTickCount();
		wprintf(L"%s OnBegin\n", m_volume->GetVolumePath());
	}

	virtual VOID  OnEnd(PVOID) override;
	
	virtual VOID  OnError(WJ_ERROR_CODE code, PVOID) override
	{
		wprintf(L"%s OnError %d\n", m_volume->GetVolumePath(), code);
	}

	virtual BOOL  Stop(PVOID) override
	{
		return FALSE;
	}

public:
	IWJLibInterface* lib;
	IWJMftIndexFile* indexfile;
	IWJVolume* m_volume;

private:
	DWORD t;
};

class CMftSearchDeletedImpl : 
	//CIntefaceImpl,
	public CHandlerImpl, 
	public IWJMftSearchHandler
	//public IWJMftSearchDeleteHandler
{
public:
	CMftSearchDeletedImpl(IWJVolume* volume):
		CHandlerImpl()
		//CIntefaceImpl()
	{
		m_volume = volume;
		filter = new TCHAR*[10];
		ZeroMemory(filter, 10 * sizeof(TCHAR*));
		filter[0] = L"*seo*";
		//filter[1] = L"*.tmp";
	};
	~CMftSearchDeletedImpl() {};

	virtual const TCHAR**  NameMatchs() override
	{
		return nullptr;// (const TCHAR**)filter;
	}

	virtual const TCHAR**  NameNotMatchs() override
	{
		return nullptr;
	}

	
	virtual INT64  MinSize() override
	{
		return -1;
	}

	virtual INT64  MaxSize() override
	{
		return -1;
	}

	virtual UCHAR  FileOrDir() override
	{
		return 0;
	}

	virtual VOID  OnMftFileInfo(IWJMftFileRecord* pFileInfo, TCHAR* path, PVOID) override
	{
		/*if (pFileInfo->IsDir() || pFileInfo->Size() < 60 * 1024 * 1024)
			return;
*/
		wprintf(L" %lld %s %s \n", pFileInfo->FileReferences(), path, pFileInfo->FileName());
		//IWJMftFileData* datastream = pFileInfo->GetDataStream();
		//for (UINT i = 0; i < datastream->ClustersCount(); i++)
		{
			//wprintf(L"    %x  %lld, %d  %d \n", datastream->Offset(), datastream->Size(), datastream->ClustersCount(), datastream->Size() / (512*0x10));// , pLcns[i] & 0x8000000000000000 ? L"----------" : L"+++++++");
		}
		//wprintf(L"\n");
	}

	//virtual VOID OnMftFileInfo(IWJMftFileRecord* pFileInfo, TCHAR* path, PVOID) override
	//{
		//wprintf(L" %x %s %s \n", count, path, pFileInfo->FileName());
		//for (UINT i = 0; i < count; i++)
		//{
		//	wprintf(L"    %llx  ", pLcns[i]);// , pLcns[i] & 0x8000000000000000 ? L"----------" : L"+++++++");
		//}
		//wprintf(L"\n");
	//}

	virtual VOID  OnBegin(PVOID Param) override
	{
		CHandlerImpl::OnBegin(Param);
	}

	virtual VOID  OnError(WJ_ERROR_CODE code, PVOID Param) override
	{
		CHandlerImpl::OnError(code, Param);
	}

	virtual BOOL  Stop(PVOID) override
	{
		return FALSE;
	}

	virtual VOID  OnEnd(PVOID Param) override
	{
		CHandlerImpl::OnEnd(Param);
	}

	static VOID EnumDeleteFiles(int idx)
	{

		IWJVolume* volume = g_lib->GetVolume(idx);
		IWJMftReader* reader = g_lib->CreateMftReader(volume);
		if (reader)
		{
			printf("VolumePath=%S\n ", volume->GetVolumePath());
			CMftSearchDeletedImpl* impl = new CMftSearchDeletedImpl(volume);
			impl->Reader = reader;
			reader->EnumDeleteFiles(impl, nullptr);
		}
		//printf("VolumeGuid=%S\n ", volume->GetVolumeGuid());
	}

	static VOID EnumFiles(int idx)
	{

		IWJVolume* volume = g_lib->GetVolume(idx);
		//printf("VolumeGuid=%S\n ", volume->GetVolumeGuid());
		printf("VolumePath=%S\n ", volume->GetVolumePath());
		CMftSearchDeletedImpl* impl = new CMftSearchDeletedImpl(volume);
		g_lib->SearchVolume(volume, impl);
	}

private:
	TCHAR** filter;
	IWJMftReader* Reader;
};

BOOL WJS_CALL Callback (CALLBACK_ACTION action, PCALLBACK_PARAM Param, PVOID p)
{
	switch (action)
	{
	case CALLBACK_ACTION_BEGIN:
		printf("begin \n");
		break;
	case CALLBACK_ACTION_END:
		printf("end \n");
		//printf("press any key exit.\n");
		break;	

	case CALLBACK_ACTION_ERROR:
		printf("error code = %d system error = %d\n", Param->code, GetLastError());
		break;
	case CALLBACK_ACTION_CONTINUE:
		return false;
	case CALLBACK_ACTION_NAMEMATCH:
		//Param->NameMatchs = (TCHAR**)p;
		break;
	case CALLBACK_ACTION_NAMENOTMATCH:
		break;
	case CALLBACK_ACTION_MINSIZE:
		break;
	case CALLBACK_ACTION_MAXSIZE:
		break;
	case CALLBACK_ACTION_FILETYPE:
		break;
	case CALLBACK_ACTION_FILEINFO:
		//WJSFileName(Param->File);
		wprintf(L" %s %s \n", Param->File->FileName(), Param->Path);
		break;
	case CALLBACK_ACTION_FILECHANGE:
		break;
	default:
		break;
	}
	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//if (is_vista_or_later())
	//{
	//	BOOL b_elevate = FALSE;
	//	IsElevated(&b_elevate);
	//	if (!b_elevate)
	//	{
	//		CLdString wstr_path;
	//		//get_app_path(wstr_path);
	//		wstr_path += L"wjslib.exe";
	//		WipeShellExecuteEx(wstr_path, L"autorun", SW_SHOWNORMAL, 0, 0, true);

	//		return 0;
	//	}
	//}
	//
	setlocale(LC_ALL,"chs");

	SetUnhandledExceptionFilter(ExceptionFilter);

	g_lib = WJSOpen();

	for (UINT i = 0; i < g_lib->GetVolumeCount(); i++)
	{
		IWJVolume* volume = g_lib->GetVolume(i);
		printf("%d = %S %d %S\n ", i, volume->GetVolumePath(), volume->GetFileSystem(), volume->GetShlDisplayName());
		//IWJMftReader* reader = g_lib->CreateMftReader(volume);
		//if (reader)
		//	printf("reader created\n");
	}

	TCHAR** filter = new TCHAR*[10];
	ZeroMemory(filter, 10 * sizeof(TCHAR*));
	/*filter[0] = L"*.tmp";
	filter[1] = L"*.temp";
	filter[2] = L"*.chk";
	filter[3] = L"*.gid";
	filter[4] = L"*.~*";
	filter[5] = L"*.---";
	filter[6] = L"*._dd";*/

	CHandlerImpl impl;
	impl.lib = g_lib;

input:
	
	printf("input index \n");
	WCHAR buffer[81] = { 0 };
	_getws_s(buffer, 80);
	if (buffer[0] == '\0')
		goto exit;
	
	int i = _wtoi(buffer);
	if (i < 0 || i >= g_lib->GetVolumeCount())
	{
		printf("input error\n");
		goto input;
	}
	
	IWJVolume* volume = g_lib->GetVolume(i);
	//WJSSearchVolume(volume, &Callback, filter);
	//CMftSearchDeletedImpl::EnumFiles(i);
	CMftSearchDeletedImpl::EnumDeleteFiles(i);

	//impl.m_volume = volume;
	//IWJMftIndexFile* file = g_lib->CreateIndexFile(volume, L"test.db", &impl, TRUE);
	//if (file == nullptr)
	//	printf("create file fail");
	//else
	//	impl.indexfile = file;

	goto input;

	getchar();

	//file->StopListener();
	//file->Close();

exit:
	WJSClose();
	return 0;
}

VOID CHandlerImpl::OnEnd(PVOID)
{
	t = GetTickCount() - t;
	wprintf(L"%s OnEnd 用时 %d\n", m_volume->GetVolumePath(), t);
	//CMftSearchDeletedImpl* impl = new CMftSearchDeletedImpl(m_volume);

	//lib->SearchIndexFile(indexfile, impl);

	//delete this;
}
