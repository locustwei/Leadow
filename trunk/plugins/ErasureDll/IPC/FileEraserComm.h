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
	�����ⲿ���̷���
	*/
	bool CallMethod(
		DWORD dwId,       //����ID
		PVOID Param,      //����ָ��
	 	WORD nParamSize,  //�����ֽ���
		PVOID* result,    //��������ֵ��null������Ҫ���أ�
		IGernalCallback<PVOID>* progress = nullptr  //��Ҫ�������ݣ��磺����״̬��
	);
};

