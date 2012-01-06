#ifndef _TSUBGET_H_
#define _TSUBGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUILD_VERSION		L"0.85PRE"
#define BUILD_COUNT			10

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
	LANGID_DEFAULT
} LangID;

typedef struct ParserOpts {
	wchar_t fileIn[MAX_PATH];
	wchar_t fileOut[MAX_PATH];
	LangID langId;
	char addColourTags;
	char overwriteOutput;
	unsigned pageNumber;	
	__int64 delay;
} ParserOpts;

typedef struct CaptionsParser CaptionsParser;

int tsgInit(CaptionsParser **p, ParserOpts *po);
int tsgProcess(CaptionsParser *p);
int tsgGetProgress(CaptionsParser *p);
void tsgSignalAbort(CaptionsParser *p);
int tsgWriteout(CaptionsParser *p);
void tsgClose(CaptionsParser **p);
void tsgGetError(int errCode, wchar_t *buf, int bufSize);

#endif

