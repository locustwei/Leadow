#pragma once

class CLdCommunication;

interface ICommunicationListen
{
	virtual bool OnCreate(CLdCommunication* Sender) = 0;
	virtual void OnTerminate(CLdCommunication* Sender) = 0;
	virtual void OnCommand(CLdCommunication* Sender) = 0;
};

class CLdCommunication
	:IGernalCallback<PVOID>
{
public:
	CLdCommunication(ICommunicationListen* listen);
	CLdCommunication(ICommunicationListen* listen, TCHAR* sharedata);
	~CLdCommunication();

	DWORD TerminateHost();
	DWORD LoadHost();
//	DWORD ExecuteFileAnalysis(CLdArray<TCHAR*>* files);
protected:
	ICommunicationListen* m_Listen;
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
	INT_PTR WaitHost(UINT_PTR Param);
	//客户进程共享数据读取回掉
	BOOL GernalCallback_Callback(void* pData, UINT_PTR Param) override;
};

