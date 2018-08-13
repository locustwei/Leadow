#pragma once

#include "../../StockDataAPI/IDataInterface.h"
#include "../ITradInterface.h"

extern "C"{
	#include "../../LuaLib/lua.h"
	#include "../../LuaLib/lauxlib.h"
	#include "../../LuaLib/lualib.h"
#include <afxcoll.h>
};

//�ű�ע�Ͳ��ֹؼ��֣�����
#define S_COMMENT_B "--[["  
#define S_COMMENT_E "--]]"
#define S_TRAD_NAME "[����]:"
#define	S_TRAD_MEMO "[˵��]:"

#define S_TARD_PARAM_ID "This"    //Ĭ�ϸ��ű����ӵĲ�����ͨ���˲����ű����Ի���û���������Ϣ

typedef struct _STRATEGY_STRCPIT  //���ײ��Խű��ṹ
{
	LPCSTR szName;         //��ʾ��
	LPSTR szFunction;      //��̬����ĺ�����
	LPCSTR szComment;      //ע��
	int nParamCount;       //������Ŀ����
	LPSTR* szParams;       //���������б�
	LPSTR* szParamComments;//����˵��
}STRATEGY_STRCPIT, *PSTRATEGY_STRCPIT;

class CScriptEng
{
	friend BOOL CALLBACK EnumResStrategy(__in  HMODULE hModule,__in  LPCTSTR lpszType,__in  LPTSTR lpszName,__in  LONG_PTR lParam);
	friend BOOL CALLBACK EnumResLib(__in  HMODULE hModule,__in  LPCTSTR lpszType,__in  LPTSTR lpszName,__in  LONG_PTR lParam);
public:
	CScriptEng(void);
	~CScriptEng(void);
public:
	CMap<CStringA, LPCSTR, PSTRATEGY_STRCPIT, PSTRATEGY_STRCPIT> m_Strategy;

	BOOL SetDataInterface(IDataInterface* iInt);   //ȡ���ӿ�
	BOOL SetTradInterface(ITradInterface* iInt);

	BOOL RunScript(LPCSTR szScript, _Out_ float* fResult);      //ִ�нű�
	BOOL RunScript(LPCSTR szScript, DWORD* dwResult);
	BOOL RunScript(LPCSTR szScript, double* fResult);
	BOOL RunScript(LPCSTR szScript, boolean* bResult);
	BOOL RunScript(LPCSTR szScript, LPCSTR* szResult);
	
	BOOL TestStrategy(PSTRATEGY_STRCPIT pStrategy, CString param ...);
private:
	lua_State* m_hLua;
	IDataInterface* m_IStockData;
	ITradInterface* m_IStockTrad;
	//CLdList<PSTRATEGY_STRCPIT> m_TradScripts;

	BOOL AddLib(LPCSTR szLib);   //���غ�����
	PSTRATEGY_STRCPIT AddFunction( LPSTR szScript);  //���ز��Ժ���
	BOOL RunScript(LPCSTR szScript);
};

