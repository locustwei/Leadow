#pragma once

class CFileEraserComm;

interface ICommunicationListen
{
	virtual bool OnCreate(CFileEraserComm* Sender) = 0;
	virtual void OnTerminate(CFileEraserComm* Sender) = 0;
	virtual void OnCommand(CFileEraserComm* Sender) = 0;
};

class CFileEraserComm
	:IGernalCallback<PVOID>
{
public:
	CFileEraserComm(ICommunicationListen* listen);
	CFileEraserComm(ICommunicationListen* listen, TCHAR* sharedata);
	~CFileEraserComm();

	DWORD TerminateHost();
	DWORD LoadHost();
//	DWORD ExecuteFileAnalysis(CLdArray<TCHAR*>* files);
protected:
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
private:
	ICommunicationListen* m_Listen;
	INT_PTR WaitHost(UINT_PTR Param);
	//�ͻ����̹������ݶ�ȡ�ص�
	BOOL GernalCallback_Callback(void* pData, UINT_PTR Param) override;
};

