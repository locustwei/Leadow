#include "stdafx.h"
#include "LdDriver.h"


CLdDriver::CLdDriver()
{
	SetDriverName();

	m_hDriver = CreateFile(m_DriverName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
}


CLdDriver::~CLdDriver()
{
	if (m_hDriver != INVALID_HANDLE_VALUE)
		CloseHandle(m_hDriver);
	m_hDriver = INVALID_HANDLE_VALUE;
}

HANDLE CLdDriver::GetHandle()
{
	return m_hDriver;
}

BOOL CLdDriver::CallDriver(DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned)
{
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	return DeviceIoControl(m_hDriver, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned, NULL);
}
