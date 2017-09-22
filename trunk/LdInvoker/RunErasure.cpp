#include "stdafx.h"
#include "RunErasure.h"
#include "../LdFileEraser/ErasureLibrary.h"
#include "Library.h"

DWORD RunEraseFile(CLdConfig& Param)
{
	IErasureLibrary* Eraser = CLdLibray::LoadErasureLibrary();
	
	return Eraser->EraseFile(Param);
}
