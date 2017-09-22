#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <Commdlg.h>
#include <shellapi.h>
#include <io.h>
#include <fcntl.h>
#include "../Jsonlib/JsonBox.h"
#include <sqlext.h>

#import "C:\\Program Files (x86)\\Common Files\\System\\ado\\msado15.dll" no_namespace rename("EOF", "ADOEOF")

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	return (int) 0;
}

/*
class CImpl: 
	public IGernalCallback<PSH_HEAD_INFO>
	,public ISortCompare<CLdString*>
{
public:
	CImpl()
	{
		Count = 0;
	};

	int Count;

	int ArraySortCompare(CLdString** it1, CLdString** it2) override
	{
		int k = _tcsicmp((*it1)->GetData(), (*it2)->GetData());
		if (k == 0)
			printf("%d\n", Count++);
		return k;
	};

	BOOL GernalCallback_Callback(_SH_HEAD_INFO* pData, UINT_PTR Param) override
	{
		CLdArray<CLdString>* columes = (CLdArray<CLdString>*)Param;
		columes->Add(pData->szName);
		//printf("%S\n", pData->szName);

		return true;
	};
};*/

// Function declarations  
inline void TESTHR(HRESULT x) { if FAILED(x) _com_issue_error(x); };
void ConnectionStringX();
_bstr_t GetState(int intState);
void PrintProviderError(_ConnectionPtr pConnection);
void PrintComError(_com_error &e);


void ConnectionStringX() {
	//https://docs.microsoft.com/en-us/sql/ado/reference/ado-api/connectionstring-connectiontimeout-and-state-properties-example-vc
	// Define Connection object pointers.  Initialize pointers on define.  These are in the ADODB::  namespace  
	_ConnectionPtr pConnection1 = NULL;

	// Define Other Variables  
	HRESULT hr = S_OK;

	try {
		// Open a connection using OLE DB syntax.  
		TESTHR(pConnection1.CreateInstance(__uuidof(Connection)));
		pConnection1->ConnectionString = "Provider='sqloledb';Data Source='bds230025471.my3w.com';"
			"Initial Catalog='bds230025471_db';UID=bds230025471;PWD=hicow3601;";
		pConnection1->ConnectionTimeout = 30;
		pConnection1->Open("", "", "", adConnectUnspecified);
		printf("cnn1 state: %S\n", (LPCTSTR)GetState(pConnection1->State));
	}
	catch (_com_error &e) {
		// Notify user of any errors.  Pass a connection pointer accessed from the Connection.  
		PrintProviderError(pConnection1);
		PrintComError(e);
	}

	// Cleanup objects before exit.  
	if (pConnection1)
		if (pConnection1->State == adStateOpen)
			pConnection1->Close();

}

_bstr_t GetState(int intState) {
	_bstr_t strState;
	switch (intState) {
	case adStateClosed:
		strState = "adStateClosed";
		break;
	case adStateOpen:
		strState = "adStateOpen";
		break;
	default:
		;
	}
	return strState;
}

void PrintProviderError(_ConnectionPtr pConnection) {
	// Print Provider Errors from Connection object.  
	// pErr is a record object in the Connection's Error collection.  
	ErrorPtr  pErr = NULL;

	if ((pConnection->Errors->Count) > 0) {
		long nCount = pConnection->Errors->Count;

		// Collection ranges from 0 to nCount -1.  
		for (long i = 0; i < nCount; i++) {
			pErr = pConnection->Errors->GetItem(i);
			printf("Error number: %x\t%s\n", pErr->Number, (LPCSTR)pErr->Description);
		}
	}
}

void PrintComError(_com_error &e) {
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());

	// Print Com errors.    
	printf("Error\n");
	printf("\tCode = %08lx\n", e.Error());
	printf("\tCode meaning = %S\n", e.ErrorMessage());
	printf("\tSource = %s\n", (LPCSTR)bstrSource);
	printf("\tDescription = %s\n", (LPCSTR)bstrDescription);
}

bool AnalEraseFileParam(LPWSTR* lpParams, int nParamCount, CLdConfig& Params)
{
#define MOTHED L"mothed"
#define FILE L"file"
#define UNDELFOLDER L"undelfolder"

	int mothed;
	for (int i = 0; i<nParamCount; i++)
	{
		if (wcsnicmp(lpParams[i], MOTHED, wcslen(MOTHED)) == 0)
		{
			LPWSTR p = wcschr(lpParams[i], ':');
			if (!p)
				return false;
			p += 1;
			mothed = CLdStringW::Try2Int(p, -1);
			if (mothed == -1)
				return false;
			Params.AddConfigObject("mothed", mothed);
		}
		else if (wcsnicmp(lpParams[i], FILE, wcslen(FILE)) == 0)
		{
			LPWSTR p = wcschr(lpParams[i], ':');
			if (!p)
				return false;
			p += 1;
			CLdString str = p;
			str.Trim();
			if (str.GetLength() < 3)
				return false;
			if (str[0] == '\"')
			{
				str.Delete(0, 1);
			}
			if (str[str.GetLength() - 1] == '\"')
				str.Delete(str.GetLength() - 1, 1);

			printf("%s\n", str.GetData());

			Params.AddArrayValue("file", str.GetData());
		}
		else if (wcsnicmp(lpParams[i], UNDELFOLDER, wcslen(UNDELFOLDER)) == 0)
		{
			Params.AddConfigObject("delfolder", true);
		}
		else
			return false;
	}
}

#define CMD_ERASE_FILE L"/erasefile"
#define CMD_ERASE_RECYCLE L"/eraserecycle"
#define CMD_ERASE_VOLUME L"/erasevolume"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");
	CoInitialize(nullptr);
	CLdStringW cmdLine = L"   /erasefile  undelfolder  mothed:3    file:c:\\abc.txt  file:\"c:\\ddd dddddd.exe\"";
	int ParamCount;
	cmdLine.Trim();
	printf("%S\n", cmdLine.GetData());
	LPWSTR* lpParamStrs = CommandLineToArgvW(cmdLine, &ParamCount);

	if (ParamCount == 0)
	{
		//goto help;
		return 0;
	}

	if (wcsicmp(lpParamStrs[0], CMD_ERASE_FILE) == 0)
	{
		if (ParamCount < 2)
		{
			//goto help
			return 0;
		}
		CLdConfig Param;
		if (!AnalEraseFileParam(&lpParamStrs[1], ParamCount - 1, Param))
		{
			//goto help;
			//return 0;
		}

		std::cout << Param.m_Config << std::endl;

		for (int i = 1; i<ParamCount; i++)
		{

		}
	}

	for (int i = 0; i<ParamCount; i++)
	{
		MessageBox(0, lpParamStrs[i], nullptr, 0);
	}

	LocalFree(lpParamStrs);
	printf("\npress any key exit");
	getchar();

	::CoUninitialize();

	return 0;
}
