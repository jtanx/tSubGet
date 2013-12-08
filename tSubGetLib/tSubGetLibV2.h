#ifndef _TSUBGET_H_
#define _TSUBGET_H_

#ifdef __cplusplus
	#define TSG_EXTERN_C extern "C"
#else
	#define TSG_EXTERN_C
#endif

#define CALLMETHOD __stdcall
#define TSG_API_DECLARE(type) TSG_EXTERN_C type CALLMETHOD

#define IsLangId(id) (id >= LANGID_DEFAULT && id < LANGID_NULL)
#define IsPageNo(no) (no <= 0x8FF)

typedef enum TSG_LangID {
	LANGID_DEFAULT,
	LANGID_CZECHSLOVAK,
	LANGID_ENGLISH,
	LANGID_ESTONIAN,
	LANGID_FRENCH,
	LANGID_GERMAN,
	LANGID_ITALIAN,
	LANGID_LETTISHLITHUANIAN,
	LANGID_POLISH,
	LANGID_PORTUGUESESPANISH,
	LANGID_ROMANIAN,
	LANGID_SERBIANCROATIANSLOVENIAN,
	LANGID_SWEDISHFINNISHHUNGARIAN,
	LANGID_TURKISH,
	LANGID_NULL
} TSG_LangID;

typedef void (*TSG_Info) (wchar_t *info);
typedef void (*TSG_Progress) (__int64 num, __int64 den, void *userData);
typedef struct TSG_Parser TSG_Parser;

TSG_API_DECLARE(HRESULT) TSG_Init(wchar_t *fileIn, TSG_Parser **pRet);
TSG_API_DECLARE(void) TSG_SetProgressCallback(TSG_Parser *parser, TSG_Progress progress, void *userData);
TSG_API_DECLARE(HRESULT) TSG_Run(TSG_Parser *parser, unsigned pageNumber, TSG_LangID langId);
TSG_API_DECLARE(void) TSG_Abort(TSG_Parser *parser);
TSG_API_DECLARE(HRESULT) TSG_WriteOutSRT(TSG_Parser *parser, wchar_t *fileOut, int bColourTags, int bOverwrite);
TSG_API_DECLARE(void) TSG_GetErrorString(HRESULT hr, wchar_t *buf, size_t numChars);
TSG_API_DECLARE(void) TSG_Free(TSG_Parser **parser);
#endif