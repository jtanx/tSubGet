

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Sun Apr 01 20:59:25 2012
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

#define TYPE_FORMAT_STRING_SIZE   129                               
#define PROC_FORMAT_STRING_SIZE   157                               
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


extern const MIDL_SERVER_INFO INullGrabberCB_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INullGrabberCB_ProxyInfo;


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

	/* Procedure SampleCB */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 10 */	NdrFcShort( 0x48 ),	/* 72 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pSample */

/* 24 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter StartTime */

/* 30 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter EndTime */

/* 36 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 38 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 40 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 42 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 44 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 46 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetAcceptedMediaType */

/* 48 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 50 */	NdrFcLong( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0x3 ),	/* 3 */
/* 56 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 64 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x1 ),	/* 1 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pType */

/* 72 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	NdrFcShort( 0x4c ),	/* Type Offset=76 */

	/* Return value */

/* 78 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 80 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetConnectedMediaType */

/* 84 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x4 ),	/* 4 */
/* 92 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x8 ),	/* 8 */
/* 98 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 100 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 102 */	NdrFcShort( 0x1 ),	/* 1 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pType */

/* 108 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 112 */	NdrFcShort( 0x4c ),	/* Type Offset=76 */

	/* Return value */

/* 114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 116 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetCallback */

/* 120 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x5 ),	/* 5 */
/* 128 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 134 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 136 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 142 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter Callback */

/* 144 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 146 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 148 */	NdrFcShort( 0x6e ),	/* Type Offset=110 */

	/* Return value */

/* 150 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 152 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 154 */	0x8,		/* FC_LONG */
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
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  4 */	NdrFcLong( 0x56a8689a ),	/* 1453877402 */
/*  8 */	NdrFcShort( 0xad4 ),	/* 2772 */
/* 10 */	NdrFcShort( 0x11ce ),	/* 4558 */
/* 12 */	0xb0,		/* 176 */
			0x3a,		/* 58 */
/* 14 */	0x0,		/* 0 */
			0x20,		/* 32 */
/* 16 */	0xaf,		/* 175 */
			0xb,		/* 11 */
/* 18 */	0xa7,		/* 167 */
			0x70,		/* 112 */
/* 20 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 22 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 24 */	
			0x11, 0x0,	/* FC_RP */
/* 26 */	NdrFcShort( 0x32 ),	/* Offset= 50 (76) */
/* 28 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 30 */	NdrFcShort( 0x8 ),	/* 8 */
/* 32 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 34 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 36 */	NdrFcShort( 0x10 ),	/* 16 */
/* 38 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 40 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 42 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (28) */
			0x5b,		/* FC_END */
/* 46 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 48 */	NdrFcLong( 0x0 ),	/* 0 */
/* 52 */	NdrFcShort( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 58 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 60 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 62 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 64 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 66 */	NdrFcShort( 0x1 ),	/* 1 */
/* 68 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 70 */	NdrFcShort( 0x40 ),	/* 64 */
/* 72 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 74 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 76 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 78 */	NdrFcShort( 0x48 ),	/* 72 */
/* 80 */	NdrFcShort( 0x0 ),	/* 0 */
/* 82 */	NdrFcShort( 0x18 ),	/* Offset= 24 (106) */
/* 84 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 86 */	NdrFcShort( 0xffcc ),	/* Offset= -52 (34) */
/* 88 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 90 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (34) */
/* 92 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 94 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 96 */	0x0,		/* 0 */
			NdrFcShort( 0xffc1 ),	/* Offset= -63 (34) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 100 */	0x0,		/* 0 */
			NdrFcShort( 0xffc9 ),	/* Offset= -55 (46) */
			0x8,		/* FC_LONG */
/* 104 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 106 */	
			0x12, 0x0,	/* FC_UP */
/* 108 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (64) */
/* 110 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 112 */	NdrFcLong( 0x8478c61 ),	/* 138906721 */
/* 116 */	NdrFcShort( 0xd22b ),	/* -11733 */
/* 118 */	NdrFcShort( 0x416b ),	/* 16747 */
/* 120 */	0x93,		/* 147 */
			0xb5,		/* 181 */
/* 122 */	0x33,		/* 51 */
			0x79,		/* 121 */
/* 124 */	0xcc,		/* 204 */
			0x2,		/* 2 */
/* 126 */	0x7a,		/* 122 */
			0x79,		/* 121 */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INullGrabberCB, ver. 0.0,
   GUID={0x08478C61,0xD22B,0x416b,{0x93,0xB5,0x33,0x79,0xCC,0x02,0x7A,0x79}} */

#pragma code_seg(".orpc")
static const unsigned short INullGrabberCB_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO INullGrabberCB_ProxyInfo =
    {
    &Object_StubDesc,
    NullGrabber__MIDL_ProcFormatString.Format,
    &INullGrabberCB_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INullGrabberCB_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    NullGrabber__MIDL_ProcFormatString.Format,
    &INullGrabberCB_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _INullGrabberCBProxyVtbl = 
{
    &INullGrabberCB_ProxyInfo,
    &IID_INullGrabberCB,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INullGrabberCB::SampleCB */
};

const CInterfaceStubVtbl _INullGrabberCBStubVtbl =
{
    &IID_INullGrabberCB,
    &INullGrabberCB_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: INullGrabber, ver. 0.0,
   GUID={0x96287A17,0x9D0A,0x4bcc,{0xAD,0x30,0x1E,0x44,0xA0,0xAB,0xE9,0xD1}} */

#pragma code_seg(".orpc")
static const unsigned short INullGrabber_FormatStringOffsetTable[] =
    {
    48,
    84,
    120
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
    ( CInterfaceProxyVtbl *) &_INullGrabberCBProxyVtbl,
    0
};

const CInterfaceStubVtbl * _NullGrabber_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INullGrabberStubVtbl,
    ( CInterfaceStubVtbl *) &_INullGrabberCBStubVtbl,
    0
};

PCInterfaceName const _NullGrabber_InterfaceNamesList[] = 
{
    "INullGrabber",
    "INullGrabberCB",
    0
};


#define _NullGrabber_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _NullGrabber, pIID, n)

int __stdcall _NullGrabber_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _NullGrabber, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _NullGrabber, 2, *pIndex )
    
}

const ExtendedProxyFileInfo NullGrabber_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _NullGrabber_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _NullGrabber_StubVtblList,
    (const PCInterfaceName * ) & _NullGrabber_InterfaceNamesList,
    0, // no delegation
    & _NullGrabber_IID_Lookup, 
    2,
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

