#include "stdafx.h"
#include "RunErasure.h"
#include "../LdFileEraser/ErasureLibrary.h"
#include "Library.h"

DWORD RunEraseFile(CLdConfig Param)
{
	IErasureLibrary* Eraser = CLdLibray::LoadErasureLibrary();
	Param.GetBoolean()
	Eraser->EraseFile(Param.GetInteger("", -1), Param.GetBoolean("", true), );
	return 0;
}
