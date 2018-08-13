//

#include "windows.h" 
#include <tchar.h>
#include <stdio.h>

//ע���̺߳��������ṹ�������߳�������MT4���̿ռ䣬���б�������Ԥ��д��MT4�����ڴ��С�
typedef struct _RemoteParam {  
	wchar_t szDllName[100];//dll����
    char szProcName[100]; // ��������   
	DWORD pGetProcess;//GetProcess������ַ
    DWORD pLoadLibrary;//LoadLibrary��������ڵ�ַ  
	DWORD dwThreadId;
} RemoteParam, * PRemoteParam;  

typedef HMODULE (__stdcall * LOADLIBRARY)(LPCTSTR);  
typedef FARPROC (__stdcall * GETPROCADDRESS) (HMODULE, LPCSTR);
typedef int (__stdcall * RUNPLUGIN)(DWORD);

//ע��MT4���̵��̺߳�������
//ע������MT4Plug.dll������RunMt4Plug������
int __stdcall threadProc(PRemoteParam param)  
{  
	if(param!=NULL){
		LOADLIBRARY pLoadLibrary = (LOADLIBRARY)(param->pLoadLibrary);
		if(!pLoadLibrary){
			//pMessageBox(NULL, param->szDllName, NULL, 0);
			return 1;
		}
		HMODULE hMt4Plug=pLoadLibrary(param->szDllName);
		if(!hMt4Plug){
			//pMessageBox(NULL, param->szDllName, NULL, 0);
			return 2;
		}
		GETPROCADDRESS pGetProcAddress=(GETPROCADDRESS)param->pGetProcess;
		if(!pGetProcAddress){
			//pMessageBox(NULL, param->szDllName, NULL, 0);
			return 3;
		}
		RUNPLUGIN pRunlugin=(RUNPLUGIN)pGetProcAddress(hMt4Plug, param->szProcName);
		if(!pRunlugin){
			//pMessageBox(NULL, param->szDllName, NULL, 0);
			return 4;
		}

		((RUNPLUGIN)pRunlugin)(param->dwThreadId);

		
		/*
		while (((GETMESSAGE)(param->pGetMessaage))(&param->msg, NULL, 0, 0))
		{
			((TRANSLATEMESSAGE)(param->pTranslateMessage))(&param->msg);
			((DISPATCHMESSAGE)(param->pDispatchMessage))(&param->msg);
		}
		*/
	}
	
    return 0;  
}  
/*
//���ݽ������Ƶõ�����ID,����ж������ʵ���Ļ������ص�һ��ö�ٵ��Ľ��̵�ID   
DWORD processNameToId(LPCTSTR lpszProcessName)  
{  
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
    PROCESSENTRY32 pe;  
    pe.dwSize = sizeof(PROCESSENTRY32);  
    if (!Process32First(hSnapshot, &pe)) {  
        printf(NULL,   
            "The frist entry of the process list has not been copyied to the buffer",   
           "Notice", MB_ICONINFORMATION | MB_OK);  
        return 0;  
    }  
    while (Process32Next(hSnapshot, &pe)) {  
        if (!strcmp(lpszProcessName, pe.szExeFile)) {  
            return pe.th32ProcessID;  
        }  
    }  
   
    return 0;  
}  
*/

//�������̷���Ȩ��   
bool enableDebugPriv(void)  
{  
    HANDLE hToken;  
    LUID sedebugnameValue;  
    TOKEN_PRIVILEGES tkp;  
    
    if (!OpenProcessToken(GetCurrentProcess(),   
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {  
        return false;  
    }  
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue)) {  
        CloseHandle(hToken);  
        return false;  
    }  
    tkp.PrivilegeCount = 1;  
    tkp.Privileges[0].Luid = sedebugnameValue;  
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) {  
        CloseHandle(hToken);  
        return false;  
    }  
    return true;  
}  
  

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwThreadSize=8192; //ע���̺߳������ֽ���������������С�����ִ�㣬С�����׳���

	if(argc<2){
		printf("��������������");
		getchar();
		return 0;
	}
		

	//����Ȩ�ޡ�ʹ��CreateProcess�����ӽ��̴˺�����ʡȥ
	//if(!enableDebugPriv()){
		//printf(NULL, _T("����Ȩ��ʧ��"), _T("����"), MB_ICONERROR | MB_OK);
		//return 0;
	//}

	/***������ע��Ľ���******/
	PROCESS_INFORMATION procInfo;
	STARTUPINFO starupInfo;

	ZeroMemory(&procInfo, sizeof(procInfo));
	ZeroMemory(&starupInfo, sizeof(starupInfo));
	starupInfo.cb=sizeof(starupInfo);
	if(!CreateProcess(argv[1], NULL, NULL, NULL, true, CREATE_SUSPENDED, NULL, NULL, &starupInfo, &procInfo)){
		printf("δ�ҵ�����");
		return 0;
	}
	HANDLE hTargetProcess = procInfo.hProcess;
	
	Sleep(1000); //�ȴ���������������
	getchar();
	
	ResumeThread(procInfo.hThread);
	return 0;

    //������������Ϊ�߳��忪��һ��洢����   
    //��������Ҫע��MEM_COMMIT | MEM_RESERVE�ڴ���������Լ�PAGE_EXECUTE_READWRITE�ڴ汣������   
    void* pRemoteThread = VirtualAllocEx(hTargetProcess, 0, dwThreadSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);  
    if (!pRemoteThread) {  
        printf("�������̷����ڴ�ʧ�� !");  
        return 0;  
    }  
    //���߳��忽��������������   
    if (!WriteProcessMemory(hTargetProcess,   pRemoteThread, &threadProc, dwThreadSize, 0)) {  
        printf("����������д���̺߳���ʧ�� !");  
        return 0;  
    }  
	
	WCHAR p[255] = {0};
	DWORD len  = GetModuleFileName(NULL, p, 255);
	for(int i=len; ;i--)
		if(p[i]=='\\')
			break;
		else
			p[i] = 0;
    //�̲߳�����ֵ   
    RemoteParam remoteData;  
    ZeroMemory(&remoteData, sizeof(RemoteParam));  
	remoteData.dwThreadId=procInfo.dwThreadId;
	HINSTANCE hKernel32 = LoadLibrary(_T("kernel32.dll"));
	wcscpy_s(remoteData.szDllName, p);
	wcscat_s(remoteData.szDllName, L"AddinTDX.dll");;
	strcpy_s(remoteData.szProcName, "RunPlugin");
	remoteData.pLoadLibrary = (DWORD)GetProcAddress(hKernel32, "LoadLibraryW"); 
	remoteData.pGetProcess = (DWORD)GetProcAddress(hKernel32, "GetProcAddress");

	
    //�̲߳���д������������   
    RemoteParam* pRemoteParam = (RemoteParam*)VirtualAllocEx( hTargetProcess , 0, sizeof(RemoteParam), MEM_COMMIT, PAGE_READWRITE); 
    if (!pRemoteParam) {  
        printf("�������̷��������ַʧ�� !");  
        return 0;  
    }

    if (!WriteProcessMemory(hTargetProcess , pRemoteParam, &remoteData, sizeof(RemoteParam), 0)) {  
        printf("����������д�����ʧ�� !");  
        return 0;  
    }  

    //���������������߳�   
    HANDLE hRemoteThread = CreateRemoteThread(hTargetProcess, NULL, 0, (DWORD (__stdcall *)(void *))pRemoteThread, pRemoteParam, 0, NULL);  
    if (!hRemoteThread) {  
        printf("��������߳�ʧ�� !");  
        return 0;   
    }  

	
	
    CloseHandle(hRemoteThread);  
	CloseHandle(hTargetProcess);
	return 0;
}

