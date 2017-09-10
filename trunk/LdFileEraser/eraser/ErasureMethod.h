#pragma once

/*
������һЩ��������

1��Pseudorandom Data (1 passes)��α������ݣ�1�δ���
2��British HMG IS5(Baseline) (1 passes)��Ӣ��Ӣ������5�����ߣ���1�δ���
3��Russian GOST P50739-95(2 passes)������˹ p50739-95��2�δ���
4��U.S.DoD 5220.22-M��8-306./E��(3 passes)������������5220.22-m(8-306./E,C&E)(3�δ���)
5��British HMG IS5(Enhanced) (3 passes)��Ӣ������IS5(��ǿ)��3�δ���
6��US Air Force 5020 (3 passes)�������վ�5020��3�δ���
7��US Army AR380-19 (3 passes)������½��ar380-19��3�δ���
8��U.S. DoD 5220.22-M��8-306./E,C&E��(7 passes)������������5220.22-m(8?-306./E,C&E)(7�δ���)
9��RCMP TSSIT OPS-II (7 passes)�����ô�ʼ��ﾯtssit ops-ii��7�δ���
10��Schneier 7 passes (7 passes)��ʩ�ζ�7��7�δ���
11��German VSITR(7 passes)���¹�vsitr��7�δ���
12��Gutmann��35 passes������������35)
13��First/last 16KB Eraseure����һ/������eraseure
*/

/*�����������*/
typedef enum ErasureMethodPassFunction {
	WriteRandom,    //�����
	WriteConstant   //����ֵ
};

/*��������һ������*/
class ErasureMethodPass
{
public:
	ErasureMethodPass()
	{
		function = WriteRandom;
		nCount = 0;
		bytes = NULL;
	};

	ErasureMethodPassFunction function;
	UINT nCount;    //д�����ݳ���
	PBYTE bytes;    //д������    
};

/*��������*/
class LDLIB_API CErasureMethod
{
public:
	CErasureMethod();
	~CErasureMethod();
	UINT GetPassCount();     //��������
	ErasureMethodPass* GetPassData(UINT nIndex);  //��������
	
	static CErasureMethod& Pseudorandom();       //α������ݣ�1�δ���
	static CErasureMethod& HMGIS5Baseline();     //British HMG IS5(Baseline) (1 passes)��Ӣ��Ӣ������5�����ߣ���1�δ���
	static CErasureMethod& GOSTP50739();         //ussian GOST P50739-95(2 passes)������˹ p50739-95��2�δ���
	static CErasureMethod& DoD_E();              //U.S.DoD 5220.22-M��8-306./E��(3 passes)������������5220.22-m(8-306./E,C&E)(3�δ���)
	static CErasureMethod& USAF5020();           //US Air Force 5020 (3 passes)�������վ�5020��3�δ���
	static CErasureMethod& USArmyAR380_19();     //US Army AR380-19 (3 passes)������½��ar380-19��3�δ���
	static CErasureMethod& HMGIS5Enhanced();     //British HMG IS5(Enhanced) (3 passes)��Ӣ������IS5(��ǿ)��3�δ���
	static CErasureMethod& DoD_EcE();            //U.S. DoD 5220.22-M��8-306./E,C&E��(7 passes)������������5220.22-m(8?-306./E,C&E)(7�δ���)
	static CErasureMethod& RCMP_TSSIT_OPS_II();  //RCMP TSSIT OPS-II (7 passes)�����ô�ʼ��ﾯtssit ops-ii��7�δ���
	static CErasureMethod& Schneier();           //Schneier 7 passes (7 passes)��ʩ�ζ�7��7�δ���
	static CErasureMethod& VSITR();              //German VSITR(7 passes)���¹�vsitr��7�δ���
	static CErasureMethod& Gutmann();            //Gutmann��35 passes������������35)
private:
	UINT nPassCount;
	ErasureMethodPass* Passes;

};



