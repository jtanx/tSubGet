

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Sat Mar 10 09:48:06 2012
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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_INullGrabberCB,0x08478C61,0xD22B,0x416b,0x93,0xB5,0x33,0x79,0xCC,0x02,0x7A,0x79);


MIDL_DEFINE_GUID(IID, IID_INullGrabber,0x96287A17,0x9D0A,0x4bcc,0xAD,0x30,0x1E,0x44,0xA0,0xAB,0xE9,0xD1);


MIDL_DEFINE_GUID(IID, LIBID_NullGrabberLib,0xed0469e0,0x171f,0x11e1,0xbd,0xdb,0x08,0x00,0x20,0x0c,0x9a,0x66);


MIDL_DEFINE_GUID(CLSID, CLSID_NullGrabber,0x3CFFEB81,0xA06E,0x4e1d,0xAF,0xAB,0x9E,0x6A,0xF6,0x67,0xD0,0x87);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



