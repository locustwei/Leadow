#pragma once

class CFileEraserComm
	:IGernalCallback<PVOID>
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
private:
	bool WaitHost(UINT_PTR Param);
	BOOL GernalCallback_Callback(void* pData, UINT_PTR Param) override;
};

