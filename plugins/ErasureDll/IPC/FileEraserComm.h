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
		�����ⲿ���̷���
		*/
	bool CallMethod(
		DWORD dwId,       //����ID
		PVOID Param,      //����ָ��
	 	WORD nParamSize,  //�����ֽ���
		PVOID* result,    //��������ֵ��null������Ҫ���أ�
		IGernalCallback<PVOID>* progress = nullptr  //��Ҫ�������ݣ��磺����״̬��
	);
protected:
	void ThreadBody(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
};

