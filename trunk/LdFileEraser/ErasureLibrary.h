#pragma once

class LDLIB_API IErasureLibrary
{
public:
	virtual CFramWnd* LibraryUI(CPaintManagerUI* pm) = 0;
private:
}; 
