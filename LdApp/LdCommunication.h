/*!
 * file LdCommunication.h
 *
 * author asa-pc
 * date ���� 2018
 *
 * brief 
	���̼�ͨѶ������Ԫ��
	ʵ�ֵ��ÿͻ����̡��ͻ���������������̼����ݴ��䣨�ڴ�ӳ���ļ�ʵ�֣����������ݺ�ص������ӿ�
 *
 * 
 */
#pragma once

/*!
 * classname
 *
 * brief ���̼�ͨѶ�����ӿڣ�ʹ����ʵ�ֽӿ���Ӧ����ͨѶ�¼���
 *
 * author asa-pc
 * date ���� 2018
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
	// Parameter:   WORD id   ���ú������
	// Parameter:   TCHAR * ProcessName  ����ִ�й��̵����ݽ�����������Ϣ�ȣ�
	// Parameter:   PVOID Param   ���ò���
	// Parameter:   WORD nParamSize �����ֽ���
	// description: ���̼�ͨѶ�������ܵ���������Ϣ�Ļص��ӿڡ�
	//************************************
	virtual void OnCommand(WORD id, TCHAR* ProcessName, PVOID Param, WORD nParamSize) = 0;
};

/*!
 * classname CLdCommunication
 *
 * brief ����ͨѶ����
 *
 * author asa-pc
 * date ���� 2018
 */
class CLdCommunication
	//:IGernalCallback<PVOID>
{
public:
	//��������������
	CLdCommunication(ICommunicationListen* listen);
	//��������������
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
		�����ⲿ���̷���
	*/
	bool CallMethod(
		WORD dwId,       //����ID
		PVOID Param,      //����ָ��
	 	WORD nParamSize,  //�����ֽ���
		PVOID* result,    //��������ֵ��null������Ҫ���أ�
		CMethodDelegate progress = nullptr  //��Ҫ�������ݣ��磺����״̬��
	);
	INT_PTR WaitHost(PVOID, UINT_PTR Param);
	virtual void DoRecvData(PCOMMUNICATE_DATA data);
	//�ͻ����̹������ݶ�ȡ�ص�
	INT_PTR ShareData_Callback(void* pData, UINT_PTR Param);
};

