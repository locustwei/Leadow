#pragma once

class LDLIB_API IErasureLibrary
{
public:
	IErasureLibrary();
	~IErasureLibrary();
	CFramWnd* LibraryUI();
private:
	CFramWnd* m_MainWnd;
}; 

IErasureLibrary LDLIB_API * API_Init(PAuthorization);
VOID LDLIB_API API_UnInit();