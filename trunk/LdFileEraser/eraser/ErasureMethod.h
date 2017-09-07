#pragma once

/*
Pseudorandom Data (1 passes)：伪随机数据（1次处理）
British HMG IS5(Baseline) (1 passes)：英国英国政府5（基线）（1次处理）
Russian GOST P50739-95(2 passes)：俄罗斯 p50739-95（2次处理）
U.S.DoD 5220.22-M（8-306./E）(3 passes)：美国国防部5220.22-m(8-306./E,C&E)(3次处理)
British HMG IS5(Enhanced) (3 passes)：英国邮政IS5(增强)（3次处理）
US Air Force 5020 (3 passes)：美国空军5020（3次处理）
US Army AR380-19 (3 passes)：美国陆军ar380-19（3次处理）
U.S. DoD 5220.22-M（8-306./E,C&E）(7 passes)：美国国防部5220.22-m(8?-306./E,C&E)(7次处理)
RCMP TSSIT OPS-II (7 passes)：加拿大皇家骑警tssit ops-ii（7次处理）
Schneier 7 passes (7 passes)：施奈尔7（7次处理）
German VSITR(7 passes)：德国vsitr（7次处理）
Gutmann（35 passes）：古特曼（35)
First/last 16KB Eraseure：第一/最后放弃eraseure
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

