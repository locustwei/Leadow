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

class CSocketListenerImpl: public ISocketListener
{
public:
	void OnConnected(CSocketBase*) override
	{
		printf("OnConnected\n");
	};
	void OnRecv(CSocketBase* s, PBYTE pData, WORD nLength) override
	{
		printf("OnRecv %s \n", (char*)pData);
	};
	void OnClosed(CSocketBase*) override
	{
		printf("OnClosed\n");
	};
	void OnAccept(CSocketBase*) override
	{
		printf("OnAccept\n");
	};
	void OnError(CSocketBase*, int code) override
	{
		printf("OnError %d\n", code);
	};
};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");
	CoInitialize(nullptr);
	
	
	CSocketListenerImpl impl;
	CLdSocket socket;
	socket.SetListener(&impl);
	socket.Listen();
	
	CSocketListenerImpl impl1;
	CLdSocket socket1;
	socket1.SetListener(&impl1);
	socket1.Connect("127.0.0.1");

	socket1.Send("ddddddddd", 10);
	socket1.Send("eeeeeeeee", 10);
	socket1.Send("fffffffff", 10);
	socket1.Send("wwwwwwwww", 10);
	socket1.Send("sssssssss", 10);
	printf("-------------------------------------");
	socket.GetClient(0)->Send("ddddddddd", 10);
	socket.GetClient(0)->Send("eeeeeeeee", 10);
	socket.GetClient(0)->Send("fffffffff", 10);
	socket.GetClient(0)->Send("wwwwwwwww", 10);
	socket.GetClient(0)->Send("sssssssss", 10);

	printf("\npress any key exit");
	getchar();

	::CoUninitialize();

	return 0;
}
