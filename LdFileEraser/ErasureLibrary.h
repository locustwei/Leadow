#pragma once

#define EPN_MOTHED L"mothed"
#define EPN_FILE L"file"
#define EPN_UNDELFOLDER L"undelfolder"

//�ļ�����״̬
enum E_FILE_STATE
{
	efs_ready,      //׼��
	efs_erasuring,  //������
	efs_erasured,   //�Ѳ���
	efs_error,      //����ʧ�ܣ��д���
	efs_abort       //ȡ������
};
//�����̶߳���
enum E_THREAD_OPTION
{
	eto_start,      //�����߳̿�ʼ   
	eto_begin,      //��ʼ�����������ļ���
	eto_completed,  //������ɣ������ļ���
	eto_progress,   //�������ȣ������ļ���
	eto_freespace,  //���̿��пռ�
	eto_track,      //ɾ�����ļ��ۼ�
	eto_finished,   //ȫ���������
	eto_analy,      //���̷���
	eto_analied     //���̷������
};

//�����̻߳ص�����
class LDLIB_API IEraserThreadCallback
{
public:
	virtual bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) = 0;
};

/*
�ļ�������̬�⵼���ӿ�
����API_Init��ȡ����ӿڡ�

*/
class LDLIB_API IErasureLibrary
{
public:
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

	virtual DWORD EraseFile(CLdConfig Param, IEraserThreadCallback * callback) = 0;
protected:
	//virtual ~IErasureLibrary() {};
};
