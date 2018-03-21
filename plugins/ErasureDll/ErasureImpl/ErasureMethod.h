#pragma once

/*
定义了一些擦除方法

1、Pseudorandom Data (1 passes)：伪随机数据（1次处理）
2、British HMG IS5(Baseline) (1 passes)：英国英国政府5（基线）（1次处理）
3、Russian GOST P50739-95(2 passes)：俄罗斯 p50739-95（2次处理）
4、U.S.DoD 5220.22-M（8-306./E）(3 passes)：美国国防部5220.22-m(8-306./E,C&E)(3次处理)
5、British HMG IS5(Enhanced) (3 passes)：英国邮政IS5(增强)（3次处理）
6、US Air Force 5020 (3 passes)：美国空军5020（3次处理）
7、US Army AR380-19 (3 passes)：美国陆军ar380-19（3次处理）
8、U.S. DoD 5220.22-M（8-306./E,C&E）(7 passes)：美国国防部5220.22-m(8?-306./E,C&E)(7次处理)
9、RCMP TSSIT OPS-II (7 passes)：加拿大皇家骑警tssit ops-ii（7次处理）
10、Schneier 7 passes (7 passes)：施奈尔7（7次处理）
11、German VSITR(7 passes)：德国vsitr（7次处理）
12、Gutmann（35 passes）：古特曼（35)
13、First/last 16KB Eraseure：第一/最后放弃eraseure
*/

/*如何生成数据*/
typedef enum ErasureMethodPassFunction {
	WriteRandom,    //随机数
	WriteConstant   //常理值
};

/*擦除方法一遍数据*/
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
	UINT nCount;    //写入数据长的
	PBYTE bytes;    //写入数据    
};
//擦除算法枚举
typedef enum ErasureMothedType
{
	em_Pseudorandom,       //伪随机数据（1次处理）
	em_HMGIS5Baseline,     //British HMG IS5(Baseline) (1 passes)：英国英国政府5（基线）（1次处理）
	em_GOSTP50739,         //ussian GOST P50739-95(2 passes)：俄罗斯 p50739-95（2次处理）
	em_DoD_E,              //U.S.DoD 5220.22-M（8-306./E）(3 passes)：美国国防部5220.22-m(8-306./E,C&E)(3次处理)
	em_USAF5020,           //US Air Force 5020 (3 passes)：美国空军5020（3次处理）
	em_USArmyAR380_19,     //US Army AR380-19 (3 passes)：美国陆军ar380-19（3次处理）
	em_HMGIS5Enhanced,     //British HMG IS5(Enhanced) (3 passes)：英国邮政IS5(增强)（3次处理）
	em_DoD_EcE,            //U.S. DoD 5220.22-M（8-306./E,C&E）(7 passes)：美国国防部5220.22-m(8?-306./E,C&E)(7次处理)
	em_RCMP_TSSIT_OPS_II,  //RCMP TSSIT OPS-II (7 passes)：加拿大皇家骑警tssit ops-ii（7次处理）
	em_Schneier,           //Schneier 7 passes (7 passes)：施奈尔7（7次处理）
	em_VSITR,              //German VSITR(7 passes)：德国vsitr（7次处理）
	em_Gutmann             //Gutmann（35 passes）：古特曼（35)
};
/*擦除方法*/
class CErasureMothed
{
public:
	CErasureMothed(ErasureMothedType mothed);
	~CErasureMothed();
	UINT GetPassCount();     //擦除几遍
	ErasureMethodPass* GetPassData(UINT nIndex);  //单遍数据
	
private:
	UINT nPassCount;
	ErasureMethodPass* Passes;
	CErasureMothed();

	void Pseudorandom();       //伪随机数据（1次处理）
	void HMGIS5Baseline();     //British HMG IS5(Baseline) (1 passes)：英国英国政府5（基线）（1次处理）
	void GOSTP50739();         //ussian GOST P50739-95(2 passes)：俄罗斯 p50739-95（2次处理）
	void DoD_E();              //U.S.DoD 5220.22-M（8-306./E）(3 passes)：美国国防部5220.22-m(8-306./E,C&E)(3次处理)
	void USAF5020();           //US Air Force 5020 (3 passes)：美国空军5020（3次处理）
	void USArmyAR380_19();     //US Army AR380-19 (3 passes)：美国陆军ar380-19（3次处理）
	void HMGIS5Enhanced();     //British HMG IS5(Enhanced) (3 passes)：英国邮政IS5(增强)（3次处理）
	void DoD_EcE();            //U.S. DoD 5220.22-M（8-306./E,C&E）(7 passes)：美国国防部5220.22-m(8?-306./E,C&E)(7次处理)
	void RCMP_TSSIT_OPS_II();  //RCMP TSSIT OPS-II (7 passes)：加拿大皇家骑警tssit ops-ii（7次处理）
	void Schneier();           //Schneier 7 passes (7 passes)：施奈尔7（7次处理）
	void VSITR();              //German VSITR(7 passes)：德国vsitr（7次处理）
	void Gutmann();            //Gutmann（35 passes）：古特曼（35)
};



