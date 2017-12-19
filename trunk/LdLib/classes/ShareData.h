#pragma once

/*
内存影像文件实现进程间数据共享。
*/

class CShareData
{
private:
	HANDLE m_Handle;
	LPBYTE m_Header;
	UINT m_Size;
	CShareData();
public:
	~CShareData();
	
	DWORD Write(UINT nPos, PVOID pData, UINT nLength);
	DWORD Read(UINT nPos, PVOID pData, UINT nLength);

	static CShareData* Create(TCHAR* pName, UINT nSize);
};

