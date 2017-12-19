#include "stdafx.h"
#include "ShareData.h"


CShareData::CShareData()
	: m_Handle(nullptr)
	, m_Header(nullptr)
	, m_Size(0)
{

}


CShareData::~CShareData()
{
	if (m_Header)
		UnmapViewOfFile(m_Header);
	if (m_Handle)
		CloseHandle(m_Handle);
}

DWORD CShareData::Write(UINT nPos, PVOID pData, UINT nLength)
{
	if (!m_Handle || !m_Header || nPos + nLength > m_Size)
		return DWORD(-1);
	MoveMemory(m_Header + nPos, pData, nLength);
	FlushViewOfFile(m_Header, m_Size);
	return 0;
}

DWORD CShareData::Read(UINT nPos, PVOID pData, UINT nLength)
{
	if (!m_Handle || !m_Header || nPos + nLength > m_Size)
		return DWORD(-1);
	MoveMemory(pData, m_Header + nPos, nLength);
	return 0;
}

CShareData* CShareData::Create(TCHAR* pName, UINT nSize)
{
	HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, nSize, pName);
	if (hMap == nullptr)
		return nullptr;
	CShareData* result = new CShareData;
	result->m_Handle = hMap;
	result->m_Header = (LPBYTE)MapViewOfFile(hMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, nSize);
	result->m_Size = nSize;
	return result;
}
