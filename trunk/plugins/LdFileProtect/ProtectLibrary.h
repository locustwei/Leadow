#pragma once

class IProtectLibrary
{
public:
	virtual CFramNotifyPump *  LibraryUI() = 0;
	virtual DWORD SelectProtectFlag(HWND hParentWnd) = 0;
	virtual BOOL ProtectFiles(DWORD dwFlags, int nFileCount, LPTSTR* lpFileNames) = 0;
	virtual BOOL DeleteFiles(LPTSTR lpFileNames, DWORD dwFlags) = 0;
private:

};
