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
	Result.Passes[1].bytes = new BYTE[Result.Passes[1].nCount]{ BYTE(~(Result.Passes[0].bytes[0])) };

	Result.Passes[2].function = WriteRandom;

	Result.Passes[3].function = WriteConstant;
	Result.Passes[3].nCount = 1;
	Result.Passes[3].bytes = new BYTE[Result.Passes[3].nCount]{ BYTE(rand() >> 8 & 0xFF) };

	Result.Passes[4].function = WriteConstant;
	Result.Passes[4].nCount = 1;
	Result.Passes[4].bytes = new BYTE[Result.Passes[4].nCount]{ BYTE(rand() >> 16 & 0xFF) };

	Result.Passes[5].function = WriteConstant;
	Result.Passes[5].nCount = 1;
	Result.Passes[5].bytes = new BYTE[Result.Passes[5].nCount]{ BYTE(~Result.Passes[5].bytes[0]) };

	Result.Passes[6].function = WriteRandom;
	
	return Result;
}

CErasureMethod& CErasureMethod::DoD_E()
{
	static CErasureMethod Result;
	Result.nPassCount = 3;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];

	Result.Passes[0].function = WriteConstant;
	Result.Passes[0].nCount = 1;
	Result.Passes[0].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[1].function = WriteConstant;
	Result.Passes[1].nCount = 1;
	Result.Passes[1].bytes = new BYTE[Result.Passes[1].nCount]{ 0xFF };

	Result.Passes[2].function = WriteRandom;
	return Result;
}

CErasureMethod& CErasureMethod::GOSTP50739()
{
	static CErasureMethod Result;
	Result.nPassCount = 2;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];

	Result.Passes[0].function = WriteConstant;
	Result.Passes[0].nCount = 1;
	Result.Passes[0].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[1].function = WriteRandom;

	return Result;
}

CErasureMethod& CErasureMethod::Gutmann()
{
	static CErasureMethod Result;
	Result.nPassCount = 1;

	Result.Passes[0].function = WriteRandom;                                   // 1
	Result.Passes[1].function = WriteRandom;
	Result.Passes[2].function = WriteRandom;
	Result.Passes[3].function = WriteRandom;

	Result.Passes[4].function = WriteConstant;
	Result.Passes[4].nCount = 1;
	Result.Passes[4].bytes = new BYTE[Result.Passes[4].nCount]{ 0x55 };                    // 5

	Result.Passes[5].function = WriteConstant;
	Result.Passes[5].nCount = 1;
	Result.Passes[5].bytes = new BYTE[Result.Passes[5].nCount]{ 0xAA };

	Result.Passes[6].function = WriteConstant;
	Result.Passes[6].nCount = 3;
	Result.Passes[6].bytes = new BYTE[Result.Passes[6].nCount]{ 0x92, 0x49, 0x24 };

	Result.Passes[7].function = WriteConstant;
	Result.Passes[7].nCount = 3;
	Result.Passes[7].bytes = new BYTE[Result.Passes[7].nCount]{ 0x49, 0x24, 0x92 };

	Result.Passes[8].function = WriteConstant;
	Result.Passes[8].nCount = 3;
	Result.Passes[8].bytes = new BYTE[Result.Passes[8].nCount]{ 0x24, 0x92, 0x49 };

	Result.Passes[9].function = WriteConstant;
	Result.Passes[9].nCount = 1;
	Result.Passes[9].bytes = new BYTE[Result.Passes[9].nCount]{ 0 };

	Result.Passes[10].function = WriteConstant;
	Result.Passes[10].nCount = 1;
	Result.Passes[10].bytes = new BYTE[Result.Passes[10].nCount]{ 0x11 };

	Result.Passes[11].function = WriteConstant;
	Result.Passes[11].nCount = 1;
	Result.Passes[11].bytes = new BYTE[Result.Passes[11].nCount]{ 0x22 };

	Result.Passes[12].function = WriteConstant;
	Result.Passes[12].nCount = 1;
	Result.Passes[12].bytes = new BYTE[Result.Passes[12].nCount]{ 0x33 };

	Result.Passes[13].function = WriteConstant;
	Result.Passes[13].nCount = 1;
	Result.Passes[13].bytes = new BYTE[Result.Passes[13].nCount]{ 0x44 };

	Result.Passes[14].function = WriteConstant;
	Result.Passes[14].nCount = 1;
	Result.Passes[14].bytes = new BYTE[Result.Passes[14].nCount]{ 0x55 };

	Result.Passes[15].function = WriteConstant;
	Result.Passes[15].nCount = 1;
	Result.Passes[15].bytes = new BYTE[Result.Passes[15].nCount]{ 0x66 };

	Result.Passes[16].function = WriteConstant;
	Result.Passes[16].nCount = 1;
	Result.Passes[16].bytes = new BYTE[Result.Passes[16].nCount]{ 0x77 };

	Result.Passes[17].function = WriteConstant;
	Result.Passes[17].nCount = 1;
	Result.Passes[17].bytes = new BYTE[Result.Passes[17].nCount]{ 0x88 };

	Result.Passes[18].function = WriteConstant;
	Result.Passes[18].nCount = 1;
	Result.Passes[18].bytes = new BYTE[Result.Passes[18].nCount]{ 0x99 };

	Result.Passes[19].function = WriteConstant;
	Result.Passes[19].nCount = 1;
	Result.Passes[19].bytes = new BYTE[Result.Passes[19].nCount]{ 0xAA };

	Result.Passes[20].function = WriteConstant;
	Result.Passes[20].nCount = 1;
	Result.Passes[20].bytes = new BYTE[Result.Passes[20].nCount]{ 0xBB };

	Result.Passes[21].function = WriteConstant;
	Result.Passes[21].nCount = 1;
	Result.Passes[21].bytes = new BYTE[Result.Passes[21].nCount]{ 0xCC };

	Result.Passes[22].function = WriteConstant;
	Result.Passes[22].nCount = 1;
	Result.Passes[22].bytes = new BYTE[Result.Passes[22].nCount]{ 0xDD };

	Result.Passes[23].function = WriteConstant;
	Result.Passes[23].nCount = 1;
	Result.Passes[23].bytes = new BYTE[Result.Passes[23].nCount]{ 0xEE };

	Result.Passes[24].function = WriteConstant;
	Result.Passes[24].nCount = 1;
	Result.Passes[24].bytes = new BYTE[Result.Passes[24].nCount]{ 0xFF };

	Result.Passes[25].function = WriteConstant;
	Result.Passes[25].nCount = 3;
	Result.Passes[25].bytes = new BYTE[Result.Passes[25].nCount]{ 0x92, 0x49, 0x24 };

	Result.Passes[26].function = WriteConstant;
	Result.Passes[26].nCount = 3;
	Result.Passes[26].bytes = new BYTE[Result.Passes[26].nCount]{ 0x49, 0x24, 0x92 };

	Result.Passes[27].function = WriteConstant;
	Result.Passes[27].nCount = 3;
	Result.Passes[27].bytes = new BYTE[Result.Passes[27].nCount]{ 0x24, 0x92, 0x49 };

	Result.Passes[28].function = WriteConstant;
	Result.Passes[28].nCount = 3;
	Result.Passes[28].bytes = new BYTE[Result.Passes[28].nCount]{ 0x6D, 0xB6, 0xDB };

	Result.Passes[29].function = WriteConstant;
	Result.Passes[29].nCount = 3;
	Result.Passes[29].bytes = new BYTE[Result.Passes[29].nCount]{ 0xB6, 0xDB, 0x6D };

	Result.Passes[30].function = WriteConstant;
	Result.Passes[30].nCount = 3;
	Result.Passes[30].bytes = new BYTE[Result.Passes[30].nCount]{ 0xDB, 0x6D, 0xB6 };

	Result.Passes[31].function = WriteRandom;
	Result.Passes[32].function = WriteRandom;
	Result.Passes[33].function = WriteRandom;
	Result.Passes[34].function = WriteRandom;                                   // 35
	return Result;
}

CErasureMethod& CErasureMethod::HMGIS5Enhanced()
{
	static CErasureMethod Result;
	Result.nPassCount = 3;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];

	Result.Passes[0].function = WriteConstant;
	Result.Passes[0].nCount = 1;
	Result.Passes[0].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[1].function = WriteConstant;
	Result.Passes[1].nCount = 1;
	Result.Passes[1].bytes = new BYTE[Result.Passes[1].nCount]{ 0x1 };

	Result.Passes[2].function = WriteRandom;
	return Result;
}

CErasureMethod& CErasureMethod::RCMP_TSSIT_OPS_II()
{
	static CErasureMethod Result;
	Result.nPassCount = 7;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];

	Result.Passes[0].function = WriteConstant;
	Result.Passes[0].nCount = 1;
	Result.Passes[0].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[1].function = WriteConstant;
	Result.Passes[1].nCount = 1;
	Result.Passes[1].bytes = new BYTE[Result.Passes[1].nCount]{ 0x1 };

	Result.Passes[2].function = WriteConstant;
	Result.Passes[2].nCount = 1;
	Result.Passes[2].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[3].function = WriteConstant;
	Result.Passes[3].nCount = 1;
	Result.Passes[3].bytes = new BYTE[Result.Passes[1].nCount]{ 0x1 };

	Result.Passes[4].function = WriteConstant;
	Result.Passes[4].nCount = 1;
	Result.Passes[4].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[5].function = WriteConstant;
	Result.Passes[5].nCount = 1;
	Result.Passes[5].bytes = new BYTE[Result.Passes[1].nCount]{ 0x1 };

	Result.Passes[6].function = WriteConstant;
	Result.Passes[6].nCount = 1;
	Result.Passes[6].bytes = new BYTE[Result.Passes[1].nCount]{ BYTE(rand() & 0xFF) };

	return Result;
}

CErasureMethod& CErasureMethod::Schneier()
{
	static CErasureMethod Result;
	Result.nPassCount = 7;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];

	Result.Passes[0].function = WriteConstant;
	Result.Passes[0].nCount = 1;
	Result.Passes[0].bytes = new BYTE[Result.Passes[1].nCount]{ 0x1 };

	Result.Passes[1].function = WriteConstant;
	Result.Passes[1].nCount = 1;
	Result.Passes[1].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[2].function = WriteRandom;
	Result.Passes[3].function = WriteRandom;
	Result.Passes[4].function = WriteRandom;
	Result.Passes[5].function = WriteRandom;
	Result.Passes[6].function = WriteRandom;

	return Result;
}

CErasureMethod& CErasureMethod::USAF5020()
{
	static CErasureMethod Result;
	Result.nPassCount = 3;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];

	Result.Passes[0].function = WriteConstant;
	Result.Passes[0].nCount = 1;
	Result.Passes[0].bytes = new BYTE[Result.Passes[0].nCount]{ BYTE(rand() & 0xFF) };

	Result.Passes[1].function = WriteConstant;
	Result.Passes[1].nCount = 1;
	Result.Passes[1].bytes = new BYTE[Result.Passes[0].nCount]{ BYTE(rand() >> 8 & 0xFF) };

	Result.Passes[2].function = WriteConstant;
	Result.Passes[2].nCount = 1;
	Result.Passes[2].bytes = new BYTE[Result.Passes[1].nCount]{ BYTE(rand() >> 16 & 0xFF) };

	return Result;
}

CErasureMethod& CErasureMethod::USArmyAR380_19()
{
	static CErasureMethod Result;
	Result.nPassCount = 3;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];
	Result.Passes[0].function = WriteRandom;

	Result.Passes[1].function = WriteConstant;
	Result.Passes[1].nCount = 1;
	Result.Passes[1].bytes = new BYTE[Result.Passes[0].nCount]{ BYTE(rand() & 0xFF) };

	Result.Passes[2].function = WriteConstant;
	Result.Passes[2].nCount = 1;
	Result.Passes[2].bytes = new BYTE[Result.Passes[1].nCount]{ BYTE(~(rand() & 0xFF)) };

	return Result;
}

CErasureMethod& CErasureMethod::VSITR()
{
	static CErasureMethod Result;
	Result.nPassCount = 7;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];

	Result.Passes[0].function = WriteConstant;
	Result.Passes[0].nCount = 1;
	Result.Passes[0].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[1].function = WriteConstant;
	Result.Passes[1].nCount = 1;
	Result.Passes[1].bytes = new BYTE[Result.Passes[1].nCount]{ 0x1 };

	Result.Passes[2].function = WriteConstant;
	Result.Passes[2].nCount = 1;
	Result.Passes[2].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[3].function = WriteConstant;
	Result.Passes[3].nCount = 1;
	Result.Passes[3].bytes = new BYTE[Result.Passes[1].nCount]{ 0x1 };

	Result.Passes[4].function = WriteConstant;
	Result.Passes[4].nCount = 1;
	Result.Passes[4].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };

	Result.Passes[5].function = WriteConstant;
	Result.Passes[5].nCount = 1;
	Result.Passes[5].bytes = new BYTE[Result.Passes[1].nCount]{ 0x1 };

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

CErasureMethod& CErasureMethod::HMGIS5Baseline()
{
	static CErasureMethod Result;
	Result.nPassCount = 1;
	Result.Passes = new ErasureMethodPass[Result.nPassCount];

	Result.Passes[0].function = WriteConstant;
	Result.Passes[0].nCount = 1;
	Result.Passes[0].bytes = new BYTE[Result.Passes[0].nCount]{ 0 };
	return Result;
}
