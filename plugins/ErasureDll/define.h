#pragma once

#include "stdafx.h"
#include "../plugin.h"

//���ý��������в����еĵ�һ��������ָ�������Ǹ�������
#define CMD_ERASE    TEXT("/erase")      //�ļ�����

//����"�ļ�����"���������в�������
#define EPN_ERASE_FILES   TEXT("files")
#define ENP_ERASE_VOLUMES TEXT("volumes")
#define CMD_ANALY_FILES   TEXT("anafile")
#define CMD_ANALY_VOLUMES TEXT("anavolume")
#define EPN_MOTHED        TEXT("mothed")
#define EPN_NAME          TEXT("name")
#define EPN_UNDELFOLDER   TEXT("undelfolder")

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

//�����̻߳ص�����
interface LDLIB_API IEraserListen
{
	//public:
	virtual bool EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue) = 0;
	virtual bool AnalyResult(TCHAR* FileName, PVOID pData) = 0;
};

interface LDLIB_API IErasure: IPluginInterface
{
	virtual DWORD EraseFile(CDynObject& Param, IEraserListen * callback) = 0;
	virtual DWORD EraseVolume(CDynObject& Param, IEraserListen * callback) = 0;
	virtual DWORD AnaFile(CDynObject& Param, IEraserListen * callback) = 0;
	virtual DWORD AnaVolume(CDynObject& Param, IEraserListen * callback) = 0;
};