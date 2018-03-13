#pragma once

class CLdCommunication;

interface ICommunicationListen
{
	virtual ~ICommunicationListen()
	{
	}

	virtual bool OnCreate() = 0;
	virtual void OnTerminate(DWORD exitcode) = 0;
	virtual void OnCommand(WORD id, PVOID data, WORD nSize) = 0;
};

class CLdCommunication
	//:IGernalCallback<PVOID>
{
public:
	//主动方建构函数
	CLdCommunication(ICommunicationListen* listen);
	//被动方建构函数
	CLdCommunication(ICommunicationListen* listen, TCHAR* sharedata);
	virtual ~CLdCommunication();

	DWORD TerminateHost();
	virtual DWORD LoadHost(TCHAR* plugid);
	bool IsConnected();

//	DWORD ExecuteFileAnalysis(CLdArray<TCHAR*>* files);
protected:
	typedef struct COMMUNICATE_DATA
	{
		WORD nSize;
		WORD commid;
		TCHAR progress[37];
		BYTE data[0];
	}*PCOMMUNICATE_DATA;

	ICommunicationListen* m_Listen;
	CShareData* m_Data;
	HANDLE m_hProcess;
	bool m_Connected;
	CLdString m_HostID;
	/*
		调用外部进程方法
	*/
	bool CallMethod(
		WORD dwId,       //方法ID
		PVOID Param,      //参数指针
	 	WORD nParamSize,  //参数字节数
		PVOID* result,    //函数返回值（null，不需要返回）
		CMethodDelegate progress = nullptr  //需要过程数据（如：进度状态）
	);
	INT_PTR WaitHost(PVOID, UINT_PTR Param);
	virtual void DoRecvData(PCOMMUNICATE_DATA data);
	//客户进程共享数据读取回掉
	INT_PTR ShareData_Callback(void* pData, UINT_PTR Param);
};

