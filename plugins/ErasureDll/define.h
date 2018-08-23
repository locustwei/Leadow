#pragma once

#include "stdafx.h"


//�������ò�������
#define EPN_FILES                TEXT("files")
#define EPN_OP_REMOVEDIR         TEXT("rd")
#define EPN_OP_METHOD            TEXT("method")

#define EPN_ERROR_CODE            TEXT("error")


#define ENP_ERASE_VOLUMES TEXT("volumes")
#define EPN_MOTHED        TEXT("method")
#define EPN_UNDELFOLDER   TEXT("undelfolder")

enum ERASER_COMM_IDS
{
	eci_anafiles
};

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
	eto_analied,    //���̷������
	eto_error,
	eto_abort
};

typedef struct TEST_VOLUME_RESULT {
	UINT64 FileCount;        //�ļ���
	UINT64 DirectoryCount;   //Ŀ¼��
	UINT64 FileTrackCount;   //MFT��ɾ���ļ��ĺۼ���
	UINT64 RecoverableCount; //ɾ���Ŀ��Ա��ָ��ļ������������ּ��ļ���С��Ϊ0��
	UINT   Writespeed;       //д�ļ��ٶȣ�1G�ֽ�����ʱ�䣬��λ���룩
	UINT   Cratespeed;       //����0�ֽ��ļ��ٶȣ�100���ļ�����ʱ�䣬��λ���룩
	UINT   Deletespeed;      //ɾ��0�ֽ��ļ����ٶȣ�100���ļ�����ʱ�䣬��λ���룩
	DWORD ErrorCode;        //����������루�����
}*PTEST_VOLUME_RESULT;

typedef struct TEST_FILE_RESULT {
	DWORD FileCount;        //�ļ���
	DWORD ADSCount;         //������
	UINT64 TotalSize;       //�ļ���Ŀ¼���ֽ���
	UINT64 ADSSizie;        //���ֽ���
	DWORD ErrorCode;        //����������루�����
}*PTEST_FILE_RESULT;
