/*!
 * file LdCommunication.h
 *
 * author asa-pc
 * date 三月 2018
 *
 * brief 
	进程间通讯基础单元。
	实现调用客户进程、客户进程生存管理、进程间数据传输（内存映像文件实现）、接收数据后回掉监听接口
 *
 * 
 */
#pragma once

#define RunOnMainThread GernalCallback_Callback
/*!
 * classname
 *
 * brief 进程间通讯监听接口，使用者实现接口响应进程通讯事件。
 *
 * author asa-pc
 * date 三月 2018
 */
interface ICommunicationListen
{
	virtual ~ICommunicationListen()
	{
	}

	virtual bool OnCreate() = 0;
	virtual void OnTerminate(DWORD exitcode) = 0;
	
	//************************************
	// Method:      OnCommand
	// Returns:     void
	// Parameter:   WORD id   调用函数编号
	// Parameter:   TCHAR * ProcessName  函数执行过程的数据交换（进度信息等）
	// Parameter:   PVOID Param   调用参数
	// Parameter:   WORD nParamSize 参数字节数
	// description: 进程间通讯，当接受到到调用信息的回掉接口。
	//************************************
	virtual void OnCommand(WORD id, CDynObject& Param) = 0;
};

/*!
 * classname CLdCommunication
 *
 * brief 进程通讯对象
 *
 * author asa-pc
 * date 三月 2018
 */
class CLdCommunication
	:IGernalCallback<PVOID>
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
	bool CallMethod(
		WORD dwId,       //方法ID
		CDynObject& Param,
		CDynObject* result = nullptr,              //函数返回值（null，不需要返回）
		ICommunicationListen* progress = nullptr  //需要过程数据（如：进度状态）
	);

	bool SendResult(
		WORD dwId,       //方法ID
		CDynObject& Param
	);

protected:
	//通讯数据结构
	typedef struct COMMUNICATE_DATA   
	{
		WORD nSize;
		WORD commid;
		WORD refer;           //调用commid的返回值
		TCHAR progress[37];
		BYTE data[0];
	}*PCOMMUNICATE_DATA;

	//OnCommand 回调数据结构
	typedef struct CALLBACK_DATA
	{
		ICommunicationListen* listener;
		COMMUNICATE_DATA commdata;
	}*PCALLBACK_DATA;

	ICommunicationListen* m_Listener;
	CShareData* m_Data;
	HANDLE m_hProcess;
	bool m_Connected;
	CLdString m_HostID;
	/*
		调用外部进程方法
	*/
	INT_PTR WaitHost(PVOID, UINT_PTR Param);
	virtual void DoRecvData(PCOMMUNICATE_DATA data, ICommunicationListen* listener);
	//客户进程共享数据读取回掉
	INT_PTR ShareData_Callback(void* pData, UINT_PTR Param);
	//回到主线程调用OnCommand
	virtual BOOL RunOnMainThread(PVOID pData, UINT_PTR Param) override;
private:
	CDynObject* m_ResultObj;
	HANDLE m_hWaitResult;
	void DoResultData(PCOMMUNICATE_DATA data);
};

