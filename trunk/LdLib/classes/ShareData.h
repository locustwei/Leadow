#pragma once

/*
�ڴ�Ӱ���ļ�ʵ�ֽ��̼����ݹ���
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

