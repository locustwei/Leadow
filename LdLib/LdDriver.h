#pragma once

class CLdDriver
{
public:
	CLdDriver();
	~CLdDriver();

	HANDLE GetHandle();
	BOOL CallDriver(
		DWORD dwIoControlCode,
		LPVOID lpInBuffer,
		DWORD nInBufferSize,
		LPVOID lpOutBuffer,
		DWORD nOutBufferSize,
		LPDWORD lpBytesReturned);
protected:
	LPCTSTR m_DriverName;
	virtual void SetDriverName();
private:
	HANDLE m_hDriver;
};

