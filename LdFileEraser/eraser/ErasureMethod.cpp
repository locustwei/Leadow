#include "stdafx.h"
#include "ErasureMethod.h"

CErasureMothed::CErasureMothed()
{
	nPassCount = 0;
	Passes = NULL;
}


CErasureMothed::CErasureMothed(ErasureMothedType mothed)
{
	nPassCount = 0;
	Passes = NULL;

	switch(mothed)
	{
	case em_Pseudorandom: 
		Pseudorandom();
		break;
	case em_HMGIS5Baseline: 
		HMGIS5Baseline();
		break;
	case em_GOSTP50739: 
		GOSTP50739();
		break;
	case em_DoD_E: 
		DoD_E();
		break;
	case em_USAF5020: 
		USAF5020();
		break;
	case em_USArmyAR380_19: 
		USArmyAR380_19();
		break;
	case em_HMGIS5Enhanced: 
		HMGIS5Enhanced();
		break;
	case em_DoD_EcE: 
		DoD_EcE();
		break;
	case em_RCMP_TSSIT_OPS_II: 
		RCMP_TSSIT_OPS_II();
		break;
	case em_Schneier: 
		Schneier();
		break;
	case em_VSITR: 
		VSITR();
		break;
	case em_Gutmann: 
		Gutmann();
		break;
	default: 
		break;
	}
}

CErasureMothed::~CErasureMothed()
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

UINT CErasureMothed::GetPassCount()
{
	return nPassCount;
}

ErasureMethodPass * CErasureMothed::GetPassData(UINT nIndex)
{
	return &Passes[nIndex];
}

void CErasureMothed::DoD_EcE()
{
	nPassCount = 7;
	Passes = new ErasureMethodPass[nPassCount];

	Passes[0].function = WriteConstant;
	Passes[0].nCount = 1;
	Passes[0].bytes = new BYTE[Passes[0].nCount]{BYTE(rand() & 0xFF)};

	Passes[1].function = WriteConstant;
	Passes[1].nCount = 1;
	Passes[1].bytes = new BYTE[Passes[1].nCount]{ BYTE(~(Passes[0].bytes[0])) };

	Passes[2].function = WriteRandom;

	Passes[3].function = WriteConstant;
	Passes[3].nCount = 1;
	Passes[3].bytes = new BYTE[Passes[3].nCount]{ BYTE(rand() >> 8 & 0xFF) };

	Passes[4].function = WriteConstant;
	Passes[4].nCount = 1;
	Passes[4].bytes = new BYTE[Passes[4].nCount]{ BYTE(rand() >> 16 & 0xFF) };

	Passes[5].function = WriteConstant;
	Passes[5].nCount = 1;
	Passes[5].bytes = new BYTE[Passes[5].nCount]{ BYTE(~Passes[5].bytes[0]) };

	Passes[6].function = WriteRandom;
}

void CErasureMothed::DoD_E()
{
	nPassCount = 3;
	Passes = new ErasureMethodPass[nPassCount];

	Passes[0].function = WriteConstant;
	Passes[0].nCount = 1;
	Passes[0].bytes = new BYTE[Passes[0].nCount]{ 0 };

	Passes[1].function = WriteConstant;
	Passes[1].nCount = 1;
	Passes[1].bytes = new BYTE[Passes[1].nCount]{ 0xFF };

	Passes[2].function = WriteRandom;
}

void CErasureMothed::GOSTP50739()
{
	nPassCount = 2;
	Passes = new ErasureMethodPass[nPassCount];

	Passes[0].function = WriteConstant;
	Passes[0].nCount = 1;
	Passes[0].bytes = new BYTE[Passes[0].nCount]{ 0 };

	Passes[1].function = WriteRandom;
}

void CErasureMothed::Gutmann()
{
	nPassCount = 1;

	Passes[0].function = WriteRandom;                                   // 1
	Passes[1].function = WriteRandom;
	Passes[2].function = WriteRandom;
	Passes[3].function = WriteRandom;

	Passes[4].function = WriteConstant;
	Passes[4].nCount = 1;
	Passes[4].bytes = new BYTE[Passes[4].nCount]{ 0x55 };                    // 5

	Passes[5].function = WriteConstant;
	Passes[5].nCount = 1;
	Passes[5].bytes = new BYTE[Passes[5].nCount]{ 0xAA };

	Passes[6].function = WriteConstant;
	Passes[6].nCount = 3;
	Passes[6].bytes = new BYTE[Passes[6].nCount]{ 0x92, 0x49, 0x24 };

	Passes[7].function = WriteConstant;
	Passes[7].nCount = 3;
	Passes[7].bytes = new BYTE[Passes[7].nCount]{ 0x49, 0x24, 0x92 };

	Passes[8].function = WriteConstant;
	Passes[8].nCount = 3;
	Passes[8].bytes = new BYTE[Passes[8].nCount]{ 0x24, 0x92, 0x49 };

	Passes[9].function = WriteConstant;
	Passes[9].nCount = 1;
	Passes[9].bytes = new BYTE[Passes[9].nCount]{ 0 };

	Passes[10].function = WriteConstant;
	Passes[10].nCount = 1;
	Passes[10].bytes = new BYTE[Passes[10].nCount]{ 0x11 };

	Passes[11].function = WriteConstant;
	Passes[11].nCount = 1;
	Passes[11].bytes = new BYTE[Passes[11].nCount]{ 0x22 };

	Passes[12].function = WriteConstant;
	Passes[12].nCount = 1;
	Passes[12].bytes = new BYTE[Passes[12].nCount]{ 0x33 };

	Passes[13].function = WriteConstant;
	Passes[13].nCount = 1;
	Passes[13].bytes = new BYTE[Passes[13].nCount]{ 0x44 };

	Passes[14].function = WriteConstant;
	Passes[14].nCount = 1;
	Passes[14].bytes = new BYTE[Passes[14].nCount]{ 0x55 };

	Passes[15].function = WriteConstant;
	Passes[15].nCount = 1;
	Passes[15].bytes = new BYTE[Passes[15].nCount]{ 0x66 };

	Passes[16].function = WriteConstant;
	Passes[16].nCount = 1;
	Passes[16].bytes = new BYTE[Passes[16].nCount]{ 0x77 };

	Passes[17].function = WriteConstant;
	Passes[17].nCount = 1;
	Passes[17].bytes = new BYTE[Passes[17].nCount]{ 0x88 };

	Passes[18].function = WriteConstant;
	Passes[18].nCount = 1;
	Passes[18].bytes = new BYTE[Passes[18].nCount]{ 0x99 };

	Passes[19].function = WriteConstant;
	Passes[19].nCount = 1;
	Passes[19].bytes = new BYTE[Passes[19].nCount]{ 0xAA };

	Passes[20].function = WriteConstant;
	Passes[20].nCount = 1;
	Passes[20].bytes = new BYTE[Passes[20].nCount]{ 0xBB };

	Passes[21].function = WriteConstant;
	Passes[21].nCount = 1;
	Passes[21].bytes = new BYTE[Passes[21].nCount]{ 0xCC };

	Passes[22].function = WriteConstant;
	Passes[22].nCount = 1;
	Passes[22].bytes = new BYTE[Passes[22].nCount]{ 0xDD };

	Passes[23].function = WriteConstant;
	Passes[23].nCount = 1;
	Passes[23].bytes = new BYTE[Passes[23].nCount]{ 0xEE };

	Passes[24].function = WriteConstant;
	Passes[24].nCount = 1;
	Passes[24].bytes = new BYTE[Passes[24].nCount]{ 0xFF };

	Passes[25].function = WriteConstant;
	Passes[25].nCount = 3;
	Passes[25].bytes = new BYTE[Passes[25].nCount]{ 0x92, 0x49, 0x24 };

	Passes[26].function = WriteConstant;
	Passes[26].nCount = 3;
	Passes[26].bytes = new BYTE[Passes[26].nCount]{ 0x49, 0x24, 0x92 };

	Passes[27].function = WriteConstant;
	Passes[27].nCount = 3;
	Passes[27].bytes = new BYTE[Passes[27].nCount]{ 0x24, 0x92, 0x49 };

	Passes[28].function = WriteConstant;
	Passes[28].nCount = 3;
	Passes[28].bytes = new BYTE[Passes[28].nCount]{ 0x6D, 0xB6, 0xDB };

	Passes[29].function = WriteConstant;
	Passes[29].nCount = 3;
	Passes[29].bytes = new BYTE[Passes[29].nCount]{ 0xB6, 0xDB, 0x6D };

	Passes[30].function = WriteConstant;
	Passes[30].nCount = 3;
	Passes[30].bytes = new BYTE[Passes[30].nCount]{ 0xDB, 0x6D, 0xB6 };

	Passes[31].function = WriteRandom;
	Passes[32].function = WriteRandom;
	Passes[33].function = WriteRandom;
	Passes[34].function = WriteRandom;                                   // 35
}

void CErasureMothed::HMGIS5Enhanced()
{
	nPassCount = 3;
	Passes = new ErasureMethodPass[nPassCount];

	Passes[0].function = WriteConstant;
	Passes[0].nCount = 1;
	Passes[0].bytes = new BYTE[Passes[0].nCount]{ 0 };

	Passes[1].function = WriteConstant;
	Passes[1].nCount = 1;
	Passes[1].bytes = new BYTE[Passes[1].nCount]{ 0x1 };

	Passes[2].function = WriteRandom;
}

void CErasureMothed::RCMP_TSSIT_OPS_II()
{
	nPassCount = 7;
	Passes = new ErasureMethodPass[nPassCount];

	Passes[0].function = WriteConstant;
	Passes[0].nCount = 1;
	Passes[0].bytes = new BYTE[Passes[0].nCount]{ 0 };

	Passes[1].function = WriteConstant;
	Passes[1].nCount = 1;
	Passes[1].bytes = new BYTE[Passes[1].nCount]{ 0x1 };

	Passes[2].function = WriteConstant;
	Passes[2].nCount = 1;
	Passes[2].bytes = new BYTE[Passes[0].nCount]{ 0 };

	Passes[3].function = WriteConstant;
	Passes[3].nCount = 1;
	Passes[3].bytes = new BYTE[Passes[1].nCount]{ 0x1 };

	Passes[4].function = WriteConstant;
	Passes[4].nCount = 1;
	Passes[4].bytes = new BYTE[Passes[0].nCount]{ 0 };

	Passes[5].function = WriteConstant;
	Passes[5].nCount = 1;
	Passes[5].bytes = new BYTE[Passes[1].nCount]{ 0x1 };

	Passes[6].function = WriteConstant;
	Passes[6].nCount = 1;
	Passes[6].bytes = new BYTE[Passes[1].nCount]{ BYTE(rand() & 0xFF) };

}

void CErasureMothed::Schneier()
{
	nPassCount = 7;
	Passes = new ErasureMethodPass[nPassCount];

	Passes[0].function = WriteConstant;
	Passes[0].nCount = 1;
	Passes[0].bytes = new BYTE[Passes[1].nCount]{ 0x1 };

	Passes[1].function = WriteConstant;
	Passes[1].nCount = 1;
	Passes[1].bytes = new BYTE[Passes[0].nCount]{ 0 };

	Passes[2].function = WriteRandom;
	Passes[3].function = WriteRandom;
	Passes[4].function = WriteRandom;
	Passes[5].function = WriteRandom;
	Passes[6].function = WriteRandom;
}

void CErasureMothed::USAF5020()
{
	nPassCount = 3;
	Passes = new ErasureMethodPass[nPassCount];

	Passes[0].function = WriteConstant;
	Passes[0].nCount = 1;
	Passes[0].bytes = new BYTE[Passes[0].nCount]{ BYTE(rand() & 0xFF) };

	Passes[1].function = WriteConstant;
	Passes[1].nCount = 1;
	Passes[1].bytes = new BYTE[Passes[0].nCount]{ BYTE(rand() >> 8 & 0xFF) };

	Passes[2].function = WriteConstant;
	Passes[2].nCount = 1;
	Passes[2].bytes = new BYTE[Passes[1].nCount]{ BYTE(rand() >> 16 & 0xFF) };

}

void CErasureMothed::USArmyAR380_19()
{
	nPassCount = 3;
	Passes = new ErasureMethodPass[nPassCount];
	Passes[0].function = WriteRandom;

	Passes[1].function = WriteConstant;
	Passes[1].nCount = 1;
	Passes[1].bytes = new BYTE[Passes[0].nCount]{ BYTE(rand() & 0xFF) };

	Passes[2].function = WriteConstant;
	Passes[2].nCount = 1;
	Passes[2].bytes = new BYTE[Passes[1].nCount]{ BYTE(~(rand() & 0xFF)) };

}

void CErasureMothed::VSITR()
{
	nPassCount = 7;
	Passes = new ErasureMethodPass[nPassCount];

	Passes[0].function = WriteConstant;
	Passes[0].nCount = 1;
	Passes[0].bytes = new BYTE[Passes[0].nCount]{ 0 };

	Passes[1].function = WriteConstant;
	Passes[1].nCount = 1;
	Passes[1].bytes = new BYTE[Passes[1].nCount]{ 0x1 };

	Passes[2].function = WriteConstant;
	Passes[2].nCount = 1;
	Passes[2].bytes = new BYTE[Passes[0].nCount]{ 0 };

	Passes[3].function = WriteConstant;
	Passes[3].nCount = 1;
	Passes[3].bytes = new BYTE[Passes[1].nCount]{ 0x1 };

	Passes[4].function = WriteConstant;
	Passes[4].nCount = 1;
	Passes[4].bytes = new BYTE[Passes[0].nCount]{ 0 };

	Passes[5].function = WriteConstant;
	Passes[5].nCount = 1;
	Passes[5].bytes = new BYTE[Passes[1].nCount]{ 0x1 };

	Passes[6].function = WriteRandom;
}

void CErasureMothed::Pseudorandom()
{
	nPassCount = 1;
	Passes = new ErasureMethodPass[nPassCount];

	Passes[0].function = WriteRandom;
}

void CErasureMothed::HMGIS5Baseline()
{
	nPassCount = 1;
	Passes = new ErasureMethodPass[nPassCount];

	Passes[0].function = WriteConstant;
	Passes[0].nCount = 1;
	Passes[0].bytes = new BYTE[Passes[0].nCount]{ 0 };
}
