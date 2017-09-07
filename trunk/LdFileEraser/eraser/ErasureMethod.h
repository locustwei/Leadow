#pragma once

/*
Pseudorandom Data (1 passes)��α������ݣ�1�δ���
British HMG IS5(Baseline) (1 passes)��Ӣ��Ӣ������5�����ߣ���1�δ���
Russian GOST P50739-95(2 passes)������˹ p50739-95��2�δ���
U.S.DoD 5220.22-M��8-306./E��(3 passes)������������5220.22-m(8-306./E,C&E)(3�δ���)
British HMG IS5(Enhanced) (3 passes)��Ӣ������IS5(��ǿ)��3�δ���
US Air Force 5020 (3 passes)�������վ�5020��3�δ���
US Army AR380-19 (3 passes)������½��ar380-19��3�δ���
U.S. DoD 5220.22-M��8-306./E,C&E��(7 passes)������������5220.22-m(8?-306./E,C&E)(7�δ���)
RCMP TSSIT OPS-II (7 passes)�����ô�ʼ��ﾯtssit ops-ii��7�δ���
Schneier 7 passes (7 passes)��ʩ�ζ�7��7�δ���
German VSITR(7 passes)���¹�vsitr��7�δ���
Gutmann��35 passes������������35)
First/last 16KB Eraseure����һ/������eraseure
*/
typedef enum ErasureMethodPassFunction {
	WriteRandom,
	WriteConstant
};

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
	UINT nCount;
	PBYTE bytes;
};

class LDLIB_API CErasureMethod
{
public:
	CErasureMethod();
	~CErasureMethod();
	UINT GetPassCount();
	ErasureMethodPass* GetPassData(UINT nIndex);

	static CErasureMethod& DoD_EcE();
	static CErasureMethod& Pseudorandom();
private:
	UINT nPassCount;
	ErasureMethodPass* Passes;

};

