#pragma once
#include "LdNamedPipe.h"
#include "LdStructs.h"

BOOL InvokeLdFunc(LD_FUNCTION_ID id, DWORD Flag, IPipeDataProvider* pDataProvider);
BOOL InvokeInProcess(LD_FUNCTION_ID id, DWORD Flag);
