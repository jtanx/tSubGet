#ifndef NULLGRABBERC_H
#define NULLGRABBERC_H

//API stuff
#ifdef __cplusplus
	#define NGC_EXTERN_C extern "C"
#else
	#define NGC_EXTERN_C 
#endif

#define CALLMETHOD __stdcall

typedef enum NGC_ErrorCodes {
	NGC_OK,
	NGC_ERROR,
	NGC_INVALID_ARGUMENTS,
	NGC_NO_INPUT_STREAM,
	NGC_COM_ERROR
} NGC_ErrorCodes;

typedef struct NGC_ErrorInfo {
	int errorCode;
	size_t bufferSize;
	wchar_t *buffer;
} NGC_ErrorInfo;

typedef struct NGC_Info NGC_Info;

typedef int (*NGC_Callback) (unsigned char* buffer, long length, __int64 timestamp, void *userData);
typedef void (*NGC_Progress) (__int64 num, __int64 den);

#define API_DECLARE(type) NGC_EXTERN_C __declspec(dllexport) type CALLMETHOD

API_DECLARE(NGC_Info*) NGC_Init(wchar_t *filepath, AM_MEDIA_TYPE *pmtAccept, NGC_ErrorInfo *error);
API_DECLARE(void) NGC_SetSampleCallback(NGC_Info *info, NGC_Callback callback, void *userData);
API_DECLARE(void) NGC_SetProgressCallback(NGC_Info *info, NGC_Progress progress);
API_DECLARE(int) NGC_Run(NGC_Info *info);
API_DECLARE(void) NGC_Abort(NGC_Info *info);
API_DECLARE(void) NGC_Free(NGC_Info **info);

#endif