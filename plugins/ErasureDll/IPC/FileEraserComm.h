#pragma once

class CFileEraserComm
{
public:
	CFileEraserComm();
	~CFileEraserComm();

	DWORD LoadHost(IGernalCallback<PVOID>* HostExit);
private:
	CShareData* m_Data;
};

