#pragma once
#include "../LdFileEraser/ErasureLibrary.h"

static class CLdLibray
{
public:
	CLdLibray();
	~CLdLibray();

protected:
	HMODULE m_hModule;
	virtual PVOID InitLib(TCHAR * pLibFile);
};

class CErasureLib: public CLdLibray
{
public:
	CErasureLib();
	~CErasureLib() {};
private:
	IErasureLibrary* m_Library;
};

class CProtectLib : public CLdLibray
{
public:
	CProtectLib() { InitLib(_T("LdFileProtect_d64.dll")); };
	~CProtectLib() {};

private:

};
