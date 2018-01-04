#pragma once

class CFileEraserComm
{
public:
	CFileEraserComm();
	~CFileEraserComm();

	DWORD LoadHost(IGernalCallback<PVOID>* HostExit);
	DWORD ExecuteFileAnalysis(CLdArray<TCHAR*>* files);
private:
	CShareData* m_Data;
	bool Call(DWORD dwId, PVOID Param, WORD nParamSize, PVOID* result);
	bool CallProgress(DWORD dwId, PVOID Param, WORD nParamSize, PVOID* result);
};

