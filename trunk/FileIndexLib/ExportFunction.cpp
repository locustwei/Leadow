#include "stdafx.h"
#include "CWJSLib.h"
#include "ExportFunction.h"

CWJSLib* g_Lib = NULL;

WJS_API IWJLibInterface*  WJSOpen()
{
	if(g_Lib == NULL)
		g_Lib = new CWJSLib();
	return g_Lib;
}

WJS_API VOID WJSClose()
{
	if (g_Lib)
	{
		delete g_Lib;
		g_Lib = NULL;
	}
}
