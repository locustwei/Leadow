#include "StdAfx.h"
#include "LdFunction.h"
#include <string>

using namespace std;

const LPCTSTR LD_DLL_FILENAME[] = 
{
	NULL,
	_T("LdFileHide.dll")            //LFI_HIDE_FILE
}; 


//************************************
// Method:    LoadDll
// FullName:  LoadDll
// Access:    public 
// Returns:   HMODULE
// Qualifier: ���ض�̬��
// Parameter: LPTSTR DllName���������붯̬���ļ���������·����
//************************************
HMODULE LoadDll(LPCTSTR DllName)
{
	TCHAR FileName[MAX_PATH] = {0};
	if(!GetModuleFileName(0, FileName, MAX_PATH))
		return NULL;
	wstring s(FileName);
	int k = s.find_last_of('\\');
	if(k == -1)
		return NULL;
	s = s.substr(0, k + 1);
	s += DllName;
	return LoadLibrary(s.c_str());
}

typedef int (*_EntryFunction)(LD_FUNCTION_ID id);

//************************************
// Method:    InvokeFunction
// FullName:  InvokeFunction
// Access:    public 
// Returns:   int
// Qualifier: ִ�ж�̬����ں�����
// Parameter: HWND hWnd �����ھ����
// Parameter: LPTSTR ParamStr ��ں�������
//************************************
int InvokeFunction(HMODULE hModle, LD_FUNCTION_ID id)
{
	_EntryFunction EntryFunction;
	EntryFunction = (_EntryFunction)GetProcAddress(hModle, "EntryFunction");
	if(EntryFunction)
		return EntryFunction(id);
	else
		return -1;
	return 0;
}

BOOL InvokeLdFunc(LD_FUNCTION_ID id, DWORD Flag, IPipeDataProvider* pDataProvider)
{
	BOOL b = FALSE;
	
	if(LFF_NEW_PROCESS & Flag)
	{
		b = InvokeInProcess(id, Flag ^ LFF_NEW_PROCESS);
	}else
	{
		HMODULE hModle = LoadDll(LD_DLL_FILENAME[id]);
		if(hModle == 0)
			return FALSE;
		b = InvokeFunction(hModle, id) == 0;
	}

	if(b && pDataProvider != NULL)
	{
		CLdNamedPipe Pipe;
		Pipe.CreateFlow(NULL, pDataProvider);
	}
	return b;
}

BOOL InvokeInProcess(LD_FUNCTION_ID id, DWORD Flag)
{
	TCHAR FileName[MAX_PATH] = {0};
	if(!GetModuleFileName(0, FileName, MAX_PATH))
		return false;
	wstring s(FileName);
	int k = s.find_last_of('\\');
	if(k == -1)
		return false;
	s = s.substr(0, k + 1);
	s += L"LdInvoker.exe ";
	s += id;
	s += L" ";
	s += (int)Flag;
	return ShellExecute(NULL, NULL, s.c_str(), NULL, NULL, SW_SHOWNORMAL) != NULL;

}
