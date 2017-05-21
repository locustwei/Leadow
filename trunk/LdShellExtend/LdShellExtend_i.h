

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun May 21 21:16:24 2017
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

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __LdShellExtend_i_h__
#define __LdShellExtend_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ILdContextMenu_FWD_DEFINED__
#define __ILdContextMenu_FWD_DEFINED__
typedef interface ILdContextMenu ILdContextMenu;

#endif 	/* __ILdContextMenu_FWD_DEFINED__ */


#ifndef __LdContextMenu_FWD_DEFINED__
#define __LdContextMenu_FWD_DEFINED__

#ifdef __cplusplus
typedef class LdContextMenu LdContextMenu;
#else
typedef struct LdContextMenu LdContextMenu;
#endif /* __cplusplus */

#endif 	/* __LdContextMenu_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ILdContextMenu_INTERFACE_DEFINED__
#define __ILdContextMenu_INTERFACE_DEFINED__

/* interface ILdContextMenu */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ILdContextMenu;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B15A85DA-2828-443D-A509-479D8A9E035D")
    ILdContextMenu : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct ILdContextMenuVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILdContextMenu * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILdContextMenu * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILdContextMenu * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ILdContextMenu * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ILdContextMenu * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ILdContextMenu * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ILdContextMenu * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } ILdContextMenuVtbl;

    interface ILdContextMenu
    {
        CONST_VTBL struct ILdContextMenuVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILdContextMenu_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILdContextMenu_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILdContextMenu_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILdContextMenu_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ILdContextMenu_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ILdContextMenu_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ILdContextMenu_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILdContextMenu_INTERFACE_DEFINED__ */



#ifndef __LdShellExtendLib_LIBRARY_DEFINED__
#define __LdShellExtendLib_LIBRARY_DEFINED__

/* library LdShellExtendLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_LdShellExtendLib;

EXTERN_C const CLSID CLSID_LdContextMenu;

#ifdef __cplusplus

class DECLSPEC_UUID("7C4429E3-0593-45DC-BF37-F2CBCEFB27F2")
LdContextMenu;
#endif
#endif /* __LdShellExtendLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


