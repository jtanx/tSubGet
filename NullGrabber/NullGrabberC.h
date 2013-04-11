#ifndef NULLGRABBERC_H
#define NULLGRABBERC_H

//API stuff
#ifdef __cplusplus
	#define NGC_EXTERN_C extern "C"
#else
	#define NGC_EXTERN_C 
#endif

#define CALLMETHOD __stdcall
#define NGC_API_DECLARE(type) NGC_EXTERN_C __declspec(dllexport) type CALLMETHOD

//SEVERITY_ERROR, CUSTOMER, FACILITY_ITF
//0x200-0x250 for NullGrabber error codes
#define NGC_NO_INPUT_STREAM			0xA0040200L 
#define NGC_UNKNOWN_COM_ERROR		0xA0040201L
#define NGC_UNEXPECTED_RUN_ERROR	0xA0040202L

typedef struct NGC_Info NGC_Info;

typedef HRESULT (*NGC_Callback) (unsigned char* buffer, long length, __int64 timestamp, void *userData);
typedef void (*NGC_Progress) (__int64 num, __int64 den, void *userData);

NGC_API_DECLARE(HRESULT) NGC_Init(wchar_t *filepath, AM_MEDIA_TYPE *pmtAccept, NGC_Info **pRet);
NGC_API_DECLARE(void) NGC_SetSampleCallback(NGC_Info *info, NGC_Callback callback, void *userData);
NGC_API_DECLARE(void) NGC_SetProgressCallback(NGC_Info *info, NGC_Progress progress, void *userData);
NGC_API_DECLARE(HRESULT) NGC_Run(NGC_Info *info);
NGC_API_DECLARE(void) NGC_Abort(NGC_Info *info);
NGC_API_DECLARE(void) NGC_Free(NGC_Info **info);

#endif