

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun May 07 14:14:56 2017
 */
/* Compiler settings for LdShellExtend.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "LdShellExtend_i.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _LdShellExtend_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } LdShellExtend_MIDL_TYPE_FORMAT_STRING;

typedef struct _LdShellExtend_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } LdShellExtend_MIDL_PROC_FORMAT_STRING;

typedef struct _LdShellExtend_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } LdShellExtend_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const LdShellExtend_MIDL_TYPE_FORMAT_STRING LdShellExtend__MIDL_TypeFormatString;
extern const LdShellExtend_MIDL_PROC_FORMAT_STRING LdShellExtend__MIDL_ProcFormatString;
extern const LdShellExtend_MIDL_EXPR_FORMAT_STRING LdShellExtend__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ILdContextMenu_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ILdContextMenu_ProxyInfo;



#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const LdShellExtend_MIDL_PROC_FORMAT_STRING LdShellExtend__MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const LdShellExtend_MIDL_TYPE_FORMAT_STRING LdShellExtend__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ILdContextMenu, ver. 0.0,
   GUID={0xB15A85DA,0x2828,0x443D,{0xA5,0x09,0x47,0x9D,0x8A,0x9E,0x03,0x5D}} */

#pragma code_seg(".orpc")
static const unsigned short ILdContextMenu_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO ILdContextMenu_ProxyInfo =
    {
    &Object_StubDesc,
    LdShellExtend__MIDL_ProcFormatString.Format,
    &ILdContextMenu_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ILdContextMenu_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    LdShellExtend__MIDL_ProcFormatString.Format,
    &ILdContextMenu_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _ILdContextMenuProxyVtbl = 
{
    0,
    &IID_ILdContextMenu,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION ILdContextMenu_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _ILdContextMenuStubVtbl =
{
    &IID_ILdContextMenu,
    &ILdContextMenu_ServerInfo,
    7,
    &ILdContextMenu_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    LdShellExtend__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x800025b, /* MIDL Version 8.0.603 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _LdShellExtend_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_ILdContextMenuProxyVtbl,
    0
};

const CInterfaceStubVtbl * const _LdShellExtend_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_ILdContextMenuStubVtbl,
    0
};

PCInterfaceName const _LdShellExtend_InterfaceNamesList[] = 
{
    "ILdContextMenu",
    0
};

const IID *  const _LdShellExtend_BaseIIDList[] = 
{
    &IID_IDispatch,
    0
};


#define _LdShellExtend_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _LdShellExtend, pIID, n)

int __stdcall _LdShellExtend_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_LdShellExtend_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo LdShellExtend_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _LdShellExtend_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _LdShellExtend_StubVtblList,
    (const PCInterfaceName * ) & _LdShellExtend_InterfaceNamesList,
    (const IID ** ) & _LdShellExtend_BaseIIDList,
    & _LdShellExtend_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

