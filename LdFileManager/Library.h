#pragma once

static class CLdLibray
{
public:
	CLdLibray();
	~CLdLibray();

	static CLdLibray* ErasureLibrary();
	static CLdLibray* ProtectLibrary();

	CFramWnd* GetUI();
protected:
	HMODULE m_hModule;
	CFramWnd* m_MainUI;
	BOOL InitLib(TCHAR* pLibFile);
private:
	static CLdLibray* m_Erasure;
	static CLdLibray* m_Protect;
};

class CErasureLib: public CLdLibray
{
public:
	CErasureLib() {};
	~CErasureLib() {};

private:

};

class CProtectLib : public CLdLibray
{
public:
	CProtectLib() {};
	~CProtectLib() {};

private:

};
