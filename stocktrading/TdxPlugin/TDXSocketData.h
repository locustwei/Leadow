/*!
 * Socket ͨѶ���ݶ���
 *ͨ������Ҷ�̬�������س���ʹ��Socket���н��̼�ͨ�š�
 * ���ļ�����������ݽṹ
 */
#pragma once

#include <Windows.h>
#include <malloc.h>
#include "..\StockDataAPI\IDataInterface.h"

#pragma warning( disable : 4200)

#define TDX_SOCKET_PORT 0x3421


enum TDX_TRAD_FUN
{
	TF_REGISTER,        //ע�ᣨ���Ӻ�ȷ����ݣ�
	TF_STOCKBY,         //��Ʊ����
	TF_STOCKBY_RET,     //��Ʊ���뷵��
	TF_STOCKSEL,        //��Ʊ����
	TF_STOCKSEL_RET,    //��������
	TF_GETZJGF,         //��ȡ���й�Ʊ��Ϣ
	TF_GETZJGF_RET      //��ȡ���й�Ʊ��Ϣ��������
};

//ͨ��������뽻�������������socketͨ�����ݽṹ
typedef struct _TDX_SOCKET_DATA
{
	TDX_TRAD_FUN ID;   //���ݱ�ʶ��ָʾdata���������ͣ�
	WORD nLen;          
	unsigned char data[0];  //���ݾ����������
}TDX_SOCKET_DATA, *PTDX_SOCKET_DATA;

//���������
typedef struct _TDX_STOCK_BY
{
	STOCK_MARK mark;
	char Code[7];
	float fPrice;
	DWORD dwVolume;
}TDX_STOCK_BY, *PTDX_STOCK_BY;

//���й�Ʊ��Ϣ
typedef struct _TDX_STOCK_GF
{
	TCHAR code[7];       //��Ʊ����
	TCHAR name[5];      //֤ȯ����
	DWORD sl;           //֤ȯ����
	DWORD kmsl;         //��������
	DWORD jmsl;         //��������
	float ckcbj;        //�ֲֳɱ���
	float mrjj;         //�������
	float dqj;          //��ǰ��
	float zxsz;         //������ֵ
	float ccyk;         //�ֲ�ӯ��
	WORD ykbl;          //ӯ������
	DWORD djsl;         //��������
	//�ɶ�����
	//����������
}TDX_STOCK_GF, *PTDX_STOCK_GF;

//�ʽ����������йɷ�
typedef struct _TDX_STOCK_ZJGF
{
	float ye;     //���
	float ky;     //����
	float sz;     //��ֵ
	float yk;     //ӯ��
	int count;    //���й�Ʊ��
	TDX_STOCK_GF gf[0]; //�ɷ���Ϣ 
}TDX_STOCK_ZJGF, *PTDX_STOCK_ZJGF;

inline PTDX_SOCKET_DATA MakeStockData(LPVOID p, TDX_TRAD_FUN id, _Inout_ int& nSize)
{
	int nLen = sizeof(TDX_SOCKET_DATA)+nSize;
	PTDX_SOCKET_DATA result = (PTDX_SOCKET_DATA)malloc(nLen);
	ZeroMemory(result, nLen);
	result->ID = id;
	result->nLen = nSize;
	if(p)
		CopyMemory(result->data, p, nSize);
	nSize = nLen;
	return result;
};
