#pragma once
#include "../LdFileEraser/ErasureLibrary.h"
#include "../LdFileProtect/ProtectLibrary.h"

static class CLdLibray
{
public:
	CLdLibray();
	~CLdLibray();

protected:
	HMODULE m_hModule;
	virtual PVOID InitLib(TCHAR * pLibFile);
	virtual VOID UnInitLib();
};

class CErasureLib: public CLdLibray
{
public:
	CErasureLib();
	~CErasureLib();
	CFramWnd* LibraryUI(CPaintManagerUI* pm);
private:
	IErasureLibrary* m_Library;
};

class CProtectLib : public CLdLibray
{
public:
	CProtectLib();
	~CProtectLib() {};
	CFramWnd* LibraryUI();
private:
	IProtectLibrary* m_Library;
};
