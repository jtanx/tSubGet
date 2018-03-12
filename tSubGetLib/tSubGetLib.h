#ifndef _TSUBGET_H_
#define _TSUBGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUILD_VERSION		L"0.9.7"
//#define BUILD_COUNT			15
#define BUILD_DATE			L"12 Mar 2018"
#define IsEventActive(hEvent) (WaitForSingleObject((hEvent),0) == WAIT_OBJECT_0)
#define IsLangId(id) (id >= LANGID_DEFAULT && id < LANGID_NULL)

#define NUM_COLOURS 8

enum ParserCodes {
	PARSER_OK,
	PARSER_E_NOCAPS,
	PARSER_E_OUT_EXISTS,
	PARSER_E_PARAMS,
	PARSER_E_FNF_IN,
	PARSER_E_IN,
	PARSER_E_OUT_DENIED,
	PARSER_E_MEM,
	PARSER_E_COM,
	PARSER_E_ABORT
};

typedef enum LangID {
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
} LangID;

typedef struct OutputFormatting {
	int fgColour[NUM_COLOURS];
} OutputFormatting;

typedef struct ParserOpts {
	wchar_t fileIn[MAX_PATH];
	wchar_t fileOut[MAX_PATH];
	wchar_t folderOut[MAX_PATH];
	OutputFormatting fmt;
	LangID langId;
	unsigned addColourTags;
	unsigned overwriteOutput;
	unsigned skipOutput;
	unsigned pageNumber;	
	__int64 delay;
} ParserOpts;

typedef struct CaptionsParser CaptionsParser;

extern const wchar_t *colourSet[8];

int tsgInit(CaptionsParser **p, ParserOpts *po);
int tsgProcess(CaptionsParser *p);
int tsgGetProgress(CaptionsParser *p);
int tsgGetPositionStr(CaptionsParser *p, wchar_t *buf, size_t bufSize);
void tsgSignalAbort(CaptionsParser *p);
int tsgWriteout(CaptionsParser *p);
void tsgClose(CaptionsParser **p);
void tsgGetError(int errCode, wchar_t *buf, int bufSize);
void tsgGetLangStr(LangID lang, wchar_t *outBuf, size_t bufSize);

#endif


