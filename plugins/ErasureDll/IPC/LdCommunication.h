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
		�����ⲿ���̷���
		*/
	bool CallMethod(
		DWORD dwId,       //����ID
		PVOID Param,      //����ָ��
	 	WORD nParamSize,  //�����ֽ���
		PVOID* result,    //��������ֵ��null������Ҫ���أ�
		IGernalCallback<PVOID>* progress = nullptr  //��Ҫ�������ݣ��磺����״̬��
	);
	INT_PTR WaitHost(UINT_PTR Param);
	//�ͻ����̹������ݶ�ȡ�ص�
	BOOL GernalCallback_Callback(void* pData, UINT_PTR Param) override;
};

