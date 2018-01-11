#pragma once

class CFileEraserComm
	: IThreadRunable
{
public:
	CFileEraserComm();
	~CFileEraserComm();

	DWORD TerminateHost();
	DWORD LoadHost(CMethodDelegate OnCreated, CMethodDelegate OnDestroy);
	DWORD ExecuteFileAnalysis(CLdArray<TCHAR*>* files);
protected:
	CShareData* m_Data;
	HANDLE m_hProcess;
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
protected:
	void ThreadBody(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
};

