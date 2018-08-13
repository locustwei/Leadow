/************************************************************************
�ű�ִ�С�����
�ű���Դ
1���⺯������ԴĿ¼SCRIPT_LIB���ṩ��Ʊ���ݡ����ס����š�΢�ŵ�C++�������ýӿ�
2�����ò��Ժ�������ԴĿ¼STRATEGY���ṩһЩĬ�ϵĽ��ײ��Խű���ֹӯ��ֹ���������ٵ�����ȣ�
3���Զ�����Խű����ļ�Ŀ¼.\Script\strategy�����û��Ա�д�Ľ��ף����̣��ű���
************************************************************************/
#include "stdafx.h"
#include "ScriptEng.h"
#include <stdio.h>

#pragma warning( disable : 4244 4305)

#define LUA_GLOBAL_IDATA "IStockData"
#define LUA_GLOBAL_ITRAD "IStockTrad"

/********************************************************************
Ƕ��ű���������ȡ��Ʊ����
*********************************************************************/
static int CallDataFunc(lua_State *L)
{
	int nError = lua_getglobal(L, LUA_GLOBAL_IDATA);
	if(!nError)
		return 0;

	int v = lua_tointeger(L, -1);
	IDataInterface* sd = (IDataInterface*)v;

	if(sd==NULL)
		return 0;
	int nParams = lua_gettop(L); /* number of arguments */
	if(nParams==0)
		return 0;
	int nId = lua_tointeger(L, 1); //����ID

	STOCK_MARK nMark;
	const char* szSymbol;
	STOCK_DATA_SIMPLE ss = {MARK_SZ,NULL,12.1,12.2,12.3,12.4,12.5,12.6,12.7};

	switch(nId){	
	case 100:
		nMark = (STOCK_MARK)lua_tointeger(L, 2);
		szSymbol = lua_tostring(L, 3);
		
		sd->GetCurrent(nMark, szSymbol, &ss);

		lua_pushnumber(L, ss.fClose);
		lua_pushnumber(L, ss.fOpen);
		lua_pushnumber(L, ss.fHigh);
		lua_pushnumber(L, ss.fLow);
		lua_pushnumber(L, ss.fCurrent);
		lua_pushnumber(L, ss.dwVolume);
		lua_pushnumber(L, ss.fAmount);
		return 7;
	}

	return 0;
}

/********************************************************************
Ƕ��ű�������ִ�й�Ʊ����
*********************************************************************/
static int CallTradFunc(lua_State *L)
{
	int nError = lua_getglobal(L, LUA_GLOBAL_ITRAD);
	if(!nError)
		return 0;

	int v = lua_tointeger(L, -1);
	ITradInterface* iTrad = (ITradInterface*)v;

	if(iTrad==NULL)
		return 0;
	int nParams = lua_gettop(L); /* number of arguments */
	if(nParams==0)
		return 0;
	int nId = lua_tointeger(L, 1); //����ID

	STOCK_MARK nMark;
	const char* szSymbol;
	BOOL bRet;
	switch(nId){	
	case 200:
		nMark = (STOCK_MARK)lua_tointeger(L, 2);
		szSymbol = lua_tostring(L, 3);
		bRet = iTrad->StockBy(nMark, szSymbol, 0, 0);

		lua_pushboolean(L, bRet);
		return 1;
	}

	return 0;
}

/************************************************************************
ö�ٽű���Դ�������Ѷ���ű�(�⺯����
/************************************************************************/
BOOL CALLBACK EnumResLib(
	__in  HMODULE hModule,
	__in  LPCTSTR lpszType,
	__in  LPTSTR lpszName,
	__in  LONG_PTR lParam
	)
{
	HRSRC hRes = FindResource(hModule, lpszName, lpszType);
	if(hRes){
		HGLOBAL hGlobal = LoadResource(hModule, hRes);
		if(hGlobal){
			LPVOID lpResource = LockResource(hGlobal);
			if(lpResource){
				DWORD nSize = SizeofResource(hModule, hRes);
				LPSTR szScript = new char[nSize + 1];
				CopyMemory((LPVOID)szScript, lpResource, nSize);
				szScript[nSize] = 0;
				((CScriptEng*)lParam)->AddLib(szScript);
				UnlockResource(hGlobal);
				delete szScript;
			}
			//GlobalFree(hGlobal);
		}
	}
	return TRUE;
}

/************************************************************************
ö�ٽű���Դ�������Ѷ���ű�(���׺�����
/************************************************************************/
BOOL CALLBACK EnumResStrategy(
	__in  HMODULE hModule,
	__in  LPCTSTR lpszType,
	__in  LPTSTR lpszName,
	__in  LONG_PTR lParam
	)
{
	HRSRC hRes = FindResource(hModule, lpszName, lpszType);
	if(hRes){
		HGLOBAL hGlobal = LoadResource(hModule, hRes);
		if(hGlobal){
			LPVOID lpResource = LockResource(hGlobal);
			if(lpResource){
				DWORD nSize = SizeofResource(hModule, hRes);
				LPSTR szScript = new char[nSize + 1];
				CopyMemory((LPVOID)szScript, lpResource, nSize);
				szScript[nSize] = 0;
				((CScriptEng*)lParam)->AddFunction(szScript);
				UnlockResource(hGlobal);
				delete szScript;
			}
		}
	}
	return TRUE;
}

CScriptEng::CScriptEng(void)
{
	m_hLua = luaL_newstate();
	luaopen_base(m_hLua);
	luaL_openlibs(m_hLua);

	lua_pushcfunction(m_hLua, CallDataFunc);
	lua_setglobal(m_hLua, "CallDataFunc");	

	lua_pushcfunction(m_hLua, CallTradFunc);
	lua_setglobal(m_hLua, "CallTradFunc");	
	//���ؽű���Դ
	EnumResourceNames(NULL, L"SCRIPT_LIB", &EnumResLib, (LONG_PTR)this);
	EnumResourceNames(NULL, L"STRATEGY", &EnumResStrategy, (LONG_PTR)this);
}


CScriptEng::~CScriptEng(void)
{
	if(m_hLua)
		lua_close(m_hLua);
}

BOOL CScriptEng::RunScript(LPCSTR szScript, float* fResult)
{
	if(!RunScript(szScript))
		return FALSE;
	if(fResult)
		*fResult = lua_tonumber(m_hLua, -1);
	return TRUE;
}

BOOL CScriptEng::RunScript(LPCSTR szScript)
{
	int nError = luaL_loadbuffer(m_hLua, szScript, strlen(szScript), NULL);
	if(nError)
		return FALSE;
	nError = lua_pcall(m_hLua, 0, 1, 0);
	if(nError)
		return FALSE;
	else
		return TRUE;
}

BOOL CScriptEng::RunScript(LPCSTR szScript, double* fResult)
{
	if(!RunScript(szScript))
		return FALSE;
	if(fResult)
		*fResult = lua_tonumber(m_hLua, -1);
	return TRUE;
}

BOOL CScriptEng::RunScript(LPCSTR szScript, DWORD* dwResult)
{
	if(!RunScript(szScript))
		return FALSE;
	if(dwResult)
		*dwResult = lua_tointeger(m_hLua, -1);
	return TRUE;
}

BOOL CScriptEng::RunScript(LPCSTR szScript, boolean* bResult)
{
	if(!RunScript(szScript))
		return FALSE;
	if(bResult)
		*bResult = lua_toboolean(m_hLua, -1);
	return TRUE;
}

BOOL CScriptEng::RunScript(LPCSTR szScript, LPCSTR* szResult)
{
	if(!RunScript(szScript))
		return FALSE;
	if(szResult)
		*szResult = lua_tostring(m_hLua, -1);
	return TRUE;
}

BOOL CScriptEng::AddLib( LPCSTR szLib )
{	
	int nError = luaL_dostring(m_hLua, szLib);
	return nError == 0;
}

BOOL CScriptEng::SetTradInterface( ITradInterface* iInt )
{
	m_IStockTrad = iInt;
	if(m_IStockTrad==NULL)
		lua_pushnil(m_hLua);
	else
		lua_pushinteger(m_hLua, (int)m_IStockTrad);
	lua_setglobal(m_hLua, LUA_GLOBAL_ITRAD);
	return TRUE;
}

BOOL CScriptEng::SetDataInterface( IDataInterface* iInt )
{
	m_IStockData = iInt;
	if(m_IStockData==NULL)
		lua_pushnil(m_hLua);
	else
		lua_pushinteger(m_hLua, (int)m_IStockData);
	lua_setglobal(m_hLua, LUA_GLOBAL_IDATA);
	return TRUE;
}

/************************************************************************
���ؽ��׽ű����ű�ͷ��ע�Ͳ��ֶ���ű������弰������Ϣ����ʽ����
--[[                    --ע�Ϳ�ʼ
[����]:�ٵ�����         --��[����]:��Ϊ�ؼ��֣���ʾ��ʾ��������
[˵��]:����Ʊ...��      --��[˵��]:��Ϊ�ؼ��֣���ʾ������˵��������������������ҵ�÷��ȡ�
[Code]:��Ʊ���롣       --��[*]:��Ϊ����������ʽ��Code����ĳ����Ʊ�����ں�����ʹ�á���������ʱ�ɳ���ֵ��
[nPercent]:��������     --ͬ��
--]]                    --ע�ͽ���

�ű����벿��

************************************************************************/
PSTRATEGY_STRCPIT CScriptEng::AddFunction( LPSTR szScript)
{
	//todo ���ܡ�MD5����ΨһID

	//��������ע�Ͳ��֡�
	LPSTR szName = NULL,         //��������
		szComment = NULL,        //����˵��
		szContent = NULL;        //������

	LPSTR* szParams = NULL,      //��������
		*szParamComment = NULL;  //����˵��
	int nPs = 0;
	LPSTR tmp = szScript, p = szScript;
	while(*p!=0){
		tmp = strchr(p, '\n');
		if(tmp==NULL)
			goto err;

		int nlen = tmp - p;
		if(strstr(p, S_TRAD_NAME)==p){
			nlen = nlen - strlen(S_TRAD_NAME);
			szName = (LPSTR)malloc(nlen + 1);
			ZeroMemory(szName, nlen + 1);
			memcpy(szName, p + strlen(S_TRAD_NAME), nlen);
		}else if(strstr(p, S_TRAD_MEMO)==p){
			nlen = nlen - strlen(S_TRAD_MEMO);
			szComment = (LPSTR)malloc(nlen + 1);
			ZeroMemory(szComment, nlen + 1);
			memcpy(szComment, p + strlen(S_TRAD_MEMO), nlen);
		}else if(strstr(p, S_COMMENT_E)==p){
			szContent = tmp + 1;
			break;
		}else if(p[0] == '['){
			nPs++;
			LPSTR p1 = strchr(p, ']');
			if(p1){
				int pLen = p1 - p - 1;
				szParams = (LPSTR*)realloc(szParams, nPs*sizeof(LPSTR));
				szParams[nPs-1] = (LPSTR)malloc(pLen + 1);
				memset(szParams[nPs-1], 0, nlen + 1);
				memcpy(szParams[nPs-1], p + 1, pLen);

				szParamComment = (LPSTR*)realloc(szParamComment, nPs*sizeof(LPSTR));
				szParamComment[nPs-1] = (LPSTR)malloc(nlen - pLen + 1);
				memset(szParamComment[nPs-1], 0, nlen -pLen + 1);
				memcpy(szParamComment[nPs-1], p + pLen + 3, nlen - pLen - 4);
			}
				
		}
		p = tmp + 1;
	}

	LPSTR szFunction = NULL,
		szP = NULL,
		szBody = NULL;

	if(szName==NULL || szContent == NULL){ 
		goto err;
	}

	//������һ����̬�ĺ������������⺯�����ظ���
	szFunction = (LPSTR)malloc(20);
	memset(szFunction, 0, 20);
	sprintf_s(szFunction, 20, "LdStockTrad%d", m_Strategy.GetCount()); 

	szP = NULL;
	int nlen = 1 + strlen(S_TARD_PARAM_ID);  //����Ĭ�ϲ���(ͨ����������ű����Ի�ȡ������ص�������Ϣ);
	for(int i=0; i<nPs; i++){
		nlen += strlen(szParams[i]) + 1;			
	}

	if(nlen){
		szP = (LPSTR)malloc(nlen);
		memset(szP, 0, nlen);
		for(int i=0; i<nPs; i++){
			strcat_s(szP, nlen, szParams[i]);
			strcat_s(szP, nlen, ",");
		}
	}
	strcat_s(szP, nlen, S_TARD_PARAM_ID);

	nlen = nlen + strlen(szContent) + strlen(szFunction) + 20;

	szBody = (LPSTR)malloc(nlen);
	memset(szBody, 0, nlen);
	if(szP)
		sprintf_s(szBody, nlen, "function %s(%s)\r\n%s\r\nend", szFunction, szP, szContent);
	else
		sprintf_s(szBody, nlen, "function %s()\r\n%s\r\nend", szFunction, szContent);

	PSTRATEGY_STRCPIT pScript = NULL;
	if(AddLib(szBody)){
		pScript = new STRATEGY_STRCPIT;
		pScript->szName = szName;
		pScript->nParamCount = nPs;
		pScript->szComment = szComment;
		pScript->szParams = szParams;
		pScript->szParamComments = szParamComment;
		pScript->szFunction = szFunction;
		//m_TradScripts.Add(pScript);
		m_Strategy.SetAt(pScript->szFunction, pScript);
	}

	if(szFunction){
		free(szFunction);
	}
	if(szP){
		free(szP);
	}
	if(szBody){
		free(szBody);
	}

	return pScript;

err:
	if(szName)
		free(szName);
	if(szComment)
		free(szComment);
	for(int i=0; i<nPs; i++){
		free(szParams[i]);
		free(szParamComment[i]);
	}
	if(szParams){
		free(szParams);
	}
	if(szParamComment){
		free(szParamComment);
	}
	return NULL;
}

BOOL CScriptEng::TestStrategy(PSTRATEGY_STRCPIT pStrategy, CString param ...)
{
	if(!RunScript(pStrategy->szFunction))
		return FALSE;
	return TRUE;
}