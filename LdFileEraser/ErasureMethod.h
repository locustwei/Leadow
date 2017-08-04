#pragma once

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

