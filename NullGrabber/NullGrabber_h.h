

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Apr 24 16:01:37 2014
 */
/* Compiler settings for NullGrabber.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
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

#ifndef __NullGrabber_h_h__
#define __NullGrabber_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INullGrabberCB_FWD_DEFINED__
#define __INullGrabberCB_FWD_DEFINED__
typedef interface INullGrabberCB INullGrabberCB;
#endif 	/* __INullGrabberCB_FWD_DEFINED__ */


#ifndef __INullGrabber_FWD_DEFINED__
#define __INullGrabber_FWD_DEFINED__
typedef interface INullGrabber INullGrabber;
#endif 	/* __INullGrabber_FWD_DEFINED__ */


#ifndef __NullGrabber_FWD_DEFINED__
#define __NullGrabber_FWD_DEFINED__

#ifdef __cplusplus
typedef class NullGrabber NullGrabber;
#else
typedef struct NullGrabber NullGrabber;
#endif /* __cplusplus */

#endif 	/* __NullGrabber_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "amstream.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __INullGrabberCB_INTERFACE_DEFINED__
#define __INullGrabberCB_INTERFACE_DEFINED__

/* interface INullGrabberCB */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_INullGrabberCB;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("08478C61-D22B-416b-93B5-3379CC027A79")
    INullGrabberCB : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SampleCB( 
            /* [in] */ IMediaSample *pSample,
            /* [in] */ REFERENCE_TIME *StartTime,
            /* [in] */ REFERENCE_TIME *EndTime) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INullGrabberCBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INullGrabberCB * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INullGrabberCB * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INullGrabberCB * This);
        
        HRESULT ( STDMETHODCALLTYPE *SampleCB )( 
            INullGrabberCB * This,
            /* [in] */ IMediaSample *pSample,
            /* [in] */ REFERENCE_TIME *StartTime,
            /* [in] */ REFERENCE_TIME *EndTime);
        
        END_INTERFACE
    } INullGrabberCBVtbl;

    interface INullGrabberCB
    {
        CONST_VTBL struct INullGrabberCBVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INullGrabberCB_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INullGrabberCB_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INullGrabberCB_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INullGrabberCB_SampleCB(This,pSample,StartTime,EndTime)	\
    ( (This)->lpVtbl -> SampleCB(This,pSample,StartTime,EndTime) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INullGrabberCB_INTERFACE_DEFINED__ */


#ifndef __INullGrabber_INTERFACE_DEFINED__
#define __INullGrabber_INTERFACE_DEFINED__

/* interface INullGrabber */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_INullGrabber;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("96287A17-9D0A-4bcc-AD30-1E44A0ABE9D1")
    INullGrabber : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetAcceptedMediaType( 
            /* [in] */ const AM_MEDIA_TYPE *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( 
            /* [out] */ AM_MEDIA_TYPE *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCallback( 
            /* [in] */ INullGrabberCB *Callback) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INullGrabberVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INullGrabber * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
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
            /* [in] */ INullGrabberCB *Callback);
        
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



#ifndef __NullGrabberLib_LIBRARY_DEFINED__
#define __NullGrabberLib_LIBRARY_DEFINED__

/* library NullGrabberLib */
/* [helpstring][uuid] */ 


EXTERN_C const IID LIBID_NullGrabberLib;

EXTERN_C const CLSID CLSID_NullGrabber;

#ifdef __cplusplus

class DECLSPEC_UUID("3CFFEB81-A06E-4e1d-AFAB-9E6AF667D087")
NullGrabber;
#endif
#endif /* __NullGrabberLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


