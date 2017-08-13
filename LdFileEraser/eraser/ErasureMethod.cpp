#include "stdafx.h"
#include "ErasureMethod.h"

CErasureMethod::CErasureMethod()
{
	nPassCount = 0;
	Passes = NULL;
}


CErasureMethod::~CErasureMethod()
{
	if (nPassCount == 0 || Passes == NULL)
		return;

	for (UINT i = 0; i < nPassCount; i++)
	{
		if(Passes[i].bytes)
			delete[] Passes[i].bytes;
	}

	delete[] Passes;
}

UINT CErasureMethod::GetPassCount()
{
	return nPassCount;
}

ErasureMethodPass * CErasureMethod::GetPassData(UINT nIndex)
{
	return &Passes[nIndex];
}

CErasureMethod& CErasureMethod::DoD_EcE()
{
	static CErasureMethod Result;
	Result.nPassCount = 7;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];

	Result.Passes[0].function = WriteConstant;
	Result.Passes[0].nCount = 1;
	Result.Passes[0].bytes = new BYTE[Result.Passes[0].nCount]{BYTE(rand() & 0xFF)};

	Result.Passes[1].function = WriteConstant;
	Result.Passes[1].nCount = 1;
	Result.Passes[1].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[2].function = WriteRandom;

	Result.Passes[3].function = WriteConstant;
	Result.Passes[3].nCount = 1;
	Result.Passes[3].bytes = new BYTE[Result.Passes[0].nCount]{ BYTE(rand() >> 8 & 0xFF) };

	Result.Passes[4].function = WriteConstant;
	Result.Passes[4].nCount = 1;
	Result.Passes[4].bytes = new BYTE[Result.Passes[0].nCount]{ BYTE(rand() >> 16 & 0xFF) };

	Result.Passes[5].function = WriteConstant;
	Result.Passes[5].nCount = 1;
	Result.Passes[5].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[6].function = WriteRandom;
	
	return Result;
}

CErasureMethod& CErasureMethod::Pseudorandom()
{
	static CErasureMethod Result;
	Result.nPassCount = 1;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];

	Result.Passes[0].function = WriteRandom;

	return Result;
}
