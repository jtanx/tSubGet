//NB This file was specifically hand-modded to make it work. 

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Wed Sep 28 18:47:24 2011
 */
/* Compiler settings for .\NullGrabber.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

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

#ifndef __NullGrabber_h_h__
#define __NullGrabber_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INullGrabber_FWD_DEFINED__
#define __INullGrabber_FWD_DEFINED__
typedef interface INullGrabber INullGrabber;
#endif 	/* __INullGrabber_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 

// {3CFFEB81-A06E-4e1d-AFAB-9E6AF667D087}
DEFINE_GUID(CLSID_NullGrabber, 
0x3cffeb81, 0xa06e, 0x4e1d, 0xaf, 0xab, 0x9e, 0x6a, 0xf6, 0x67, 0xd0, 0x87);

// {96287A17-9D0A-4bcc-AD30-1E44A0ABE9D1}
DEFINE_GUID(IID_INullGrabber, 
0x96287a17, 0x9d0a, 0x4bcc, 0xad, 0x30, 0x1e, 0x44, 0xa0, 0xab, 0xe9, 0xd1);

/* interface __MIDL_itf_NullGrabber_0000_0000 */
/* [local] */ 


typedef HRESULT (*NGCALLBACK) (
    IMediaSample * pSample, 
    REFERENCE_TIME * StartTime, 
    REFERENCE_TIME * StopTime );

extern RPC_IF_HANDLE __MIDL_itf_NullGrabber_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_NullGrabber_0000_0000_v0_0_s_ifspec;

#ifndef __INullGrabber_INTERFACE_DEFINED__
#define __INullGrabber_INTERFACE_DEFINED__

/* interface INullGrabber */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_INullGrabber;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4549d97d-e8ef-4fb4-896e-2f7f867eca2f")
    INullGrabber : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetAcceptedMediaType( 
            /* [in] */ const AM_MEDIA_TYPE *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( 
            /* [out] */ AM_MEDIA_TYPE *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCallback( 
            /* [in] */ NGCALLBACK Callback) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INullGrabberVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INullGrabber * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INullGrabber * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INullGrabber * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetAcceptedMediaType )( 
            INullGrabber * This,
            /* [in] */ const AM_MEDIA_TYPE *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetConnectedMediaType )( 
            INullGrabber * This,
            /* [out] */ AM_MEDIA_TYPE *pType);
        
        HRESULT ( STDMETHODCALLTYPE *SetCallback )( 
            INullGrabber * This,
            /* [in] */ NGCALLBACK Callback);
        
        END_INTERFACE
    } INullGrabberVtbl;

    interface INullGrabber
    {
        CONST_VTBL struct INullGrabberVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INullGrabber_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INullGrabber_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INullGrabber_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INullGrabber_SetAcceptedMediaType(This,pType)	\
    ( (This)->lpVtbl -> SetAcceptedMediaType(This,pType) ) 

#define INullGrabber_GetConnectedMediaType(This,pType)	\
    ( (This)->lpVtbl -> GetConnectedMediaType(This,pType) ) 

#define INullGrabber_SetCallback(This,Callback)	\
    ( (This)->lpVtbl -> SetCallback(This,Callback) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INullGrabber_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

