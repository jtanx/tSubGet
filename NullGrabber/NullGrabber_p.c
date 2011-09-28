

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Wed Sep 28 18:00:18 2011
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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "NullGrabber_h.h"

#define TYPE_FORMAT_STRING_SIZE   81                                
#define PROC_FORMAT_STRING_SIZE   109                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _NullGrabber_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } NullGrabber_MIDL_TYPE_FORMAT_STRING;

typedef struct _NullGrabber_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } NullGrabber_MIDL_PROC_FORMAT_STRING;

typedef struct _NullGrabber_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } NullGrabber_MIDL_EXPR_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const NullGrabber_MIDL_TYPE_FORMAT_STRING NullGrabber__MIDL_TypeFormatString;
extern const NullGrabber_MIDL_PROC_FORMAT_STRING NullGrabber__MIDL_ProcFormatString;
extern const NullGrabber_MIDL_EXPR_FORMAT_STRING NullGrabber__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INullGrabber_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INullGrabber_ProxyInfo;



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const NullGrabber_MIDL_PROC_FORMAT_STRING NullGrabber__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure SetAcceptedMediaType */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pType */

/* 24 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	NdrFcShort( 0x2a ),	/* Type Offset=42 */

	/* Return value */

/* 30 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetConnectedMediaType */

/* 36 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x4 ),	/* 4 */
/* 44 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x8 ),	/* 8 */
/* 50 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 52 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pType */

/* 60 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 62 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 64 */	NdrFcShort( 0x2a ),	/* Type Offset=42 */

	/* Return value */

/* 66 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 68 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 70 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetCallback */

/* 72 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 74 */	NdrFcLong( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0x5 ),	/* 5 */
/* 80 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 82 */	NdrFcShort( 0x1c ),	/* 28 */
/* 84 */	NdrFcShort( 0x8 ),	/* 8 */
/* 86 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 88 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 90 */	NdrFcShort( 0x0 ),	/* 0 */
/* 92 */	NdrFcShort( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Callback */

/* 96 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 98 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 104 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const NullGrabber_MIDL_TYPE_FORMAT_STRING NullGrabber__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x26 ),	/* Offset= 38 (42) */
/*  6 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/*  8 */	NdrFcShort( 0x8 ),	/* 8 */
/* 10 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 12 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 14 */	NdrFcShort( 0x10 ),	/* 16 */
/* 16 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 18 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 20 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (6) */
			0x5b,		/* FC_END */
/* 24 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 26 */	NdrFcLong( 0x0 ),	/* 0 */
/* 30 */	NdrFcShort( 0x0 ),	/* 0 */
/* 32 */	NdrFcShort( 0x0 ),	/* 0 */
/* 34 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 36 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 38 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 40 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 42 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 44 */	NdrFcShort( 0x48 ),	/* 72 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x18 ),	/* Offset= 24 (72) */
/* 50 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 52 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (12) */
/* 54 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 56 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (12) */
/* 58 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 60 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 62 */	0x0,		/* 0 */
			NdrFcShort( 0xffcd ),	/* Offset= -51 (12) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 66 */	0x0,		/* 0 */
			NdrFcShort( 0xffd5 ),	/* Offset= -43 (24) */
			0x8,		/* FC_LONG */
/* 70 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 72 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 74 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 76 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 78 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };


/* Standard interface: __MIDL_itf_NullGrabber_0000_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INullGrabber, ver. 0.0,
   GUID={0x4549d97d,0xe8ef,0x4fb4,{0x89,0x6e,0x2f,0x7f,0x86,0x7e,0xca,0x2f}} */

#pragma code_seg(".orpc")
static const unsigned short INullGrabber_FormatStringOffsetTable[] =
    {
    0,
    36,
    72
    };

static const MIDL_STUBLESS_PROXY_INFO INullGrabber_ProxyInfo =
    {
    &Object_StubDesc,
    NullGrabber__MIDL_ProcFormatString.Format,
    &INullGrabber_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INullGrabber_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    NullGrabber__MIDL_ProcFormatString.Format,
    &INullGrabber_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _INullGrabberProxyVtbl = 
{
    &INullGrabber_ProxyInfo,
    &IID_INullGrabber,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INullGrabber::SetAcceptedMediaType */ ,
    (void *) (INT_PTR) -1 /* INullGrabber::GetConnectedMediaType */ ,
    (void *) (INT_PTR) -1 /* INullGrabber::SetCallback */
};

const CInterfaceStubVtbl _INullGrabberStubVtbl =
{
    &IID_INullGrabber,
    &INullGrabber_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
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
    NullGrabber__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x70001f4, /* MIDL Version 7.0.500 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * _NullGrabber_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INullGrabberProxyVtbl,
    0
};

const CInterfaceStubVtbl * _NullGrabber_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INullGrabberStubVtbl,
    0
};

PCInterfaceName const _NullGrabber_InterfaceNamesList[] = 
{
    "INullGrabber",
    0
};


#define _NullGrabber_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _NullGrabber, pIID, n)

int __stdcall _NullGrabber_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_NullGrabber_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo NullGrabber_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _NullGrabber_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _NullGrabber_StubVtblList,
    (const PCInterfaceName * ) & _NullGrabber_InterfaceNamesList,
    0, // no delegation
    & _NullGrabber_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

