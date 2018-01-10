#pragma once

class CFileEraserComm
{
public:
	CFileEraserComm();
	~CFileEraserComm();

	DWORD LoadHost(IGernalCallback<PVOID>* HostExit);
	DWORD ExecuteFileAnalysis(CLdArray<TCHAR*>* files);
protected:
	CShareData* m_Data;
	/*
	调用外部进程方法
	*/
	bool CallMethod(
		DWORD dwId,       //方法ID
		PVOID Param,      //参数指针
	 	WORD nParamSize,  //参数字节数
		PVOID* result,    //函数返回值（null，不需要返回）
		IGernalCallback<PVOID>* progress = nullptr  //需要过程数据（如：进度状态）
	);
};

