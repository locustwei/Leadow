#pragma once
/*
�ļ�������̬�⵼���ӿ�
����API_Init��ȡ����ӿڡ�

*/
class LDLIB_API IErasureLibrary
{
public:
	virtual ~IErasureLibrary() {};
	/*������ԴID
	*����builer����*/
	virtual TCHAR* UIResorce() = 0;
	/*
	����Build ��ɺ�ش���Library(�ڶ�̬����Build���ڣ�ò�Ʋ�̫�ȶ�)
	*/
	virtual void SetUI(CControlUI*) = 0;
	virtual CControlUI* GetUI() = 0;
	/*
	��ȡ������Ϣ������󣨴���Build�ú������Ϣ�������
	*/
	virtual CFramNotifyPump* GetNotifyPump() = 0;
private:
}; 
