#ifndef _TSUBGET_H_
#define _TSUBGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define IsEventActive(hEvent) (WaitForSingleObject((hEvent),0) == WAIT_OBJECT_0)

typedef struct _CaptionCluster CaptionCluster;
typedef struct _FileReader FileReader;

typedef struct _ArrayInfo {
	unsigned count;
	unsigned index;
} ArrayInfo;

typedef struct _Sample {
	unsigned char *pBuf;
	__int64 time;
	long bufSize;
} Sample;

typedef struct _CaptionsParser {
	FileReader *fr;
	FILE *fpOut;
	wchar_t fileIn[MAX_PATH];
	wchar_t fileOut[MAX_PATH];

	CaptionCluster *cc;
	ArrayInfo ccI;

	HANDLE hAbort;

	unsigned overwriteOutput;
	unsigned colouredOutput;
	unsigned lang;
	unsigned pageNum;
	__int64 delay;
} CaptionsParser;

enum _ParserCodes {
	PARSER_OK,
	PARSER_W_NOCAPS,
	PARSER_W_OUT_EXISTS,
	PARSER_E_PARAMS,
	PARSER_E_FNF_IN,
	PARSER_E_IN,
	PARSER_E_OUT_DENIED,
	PARSER_E_MEM,
	PARSER_E_COM,
	PARSER_E_ABORT
};

//Prototypes
int parserInit(CaptionsParser *p);
int parserReadFile(CaptionsParser *p);
int parserGetProgress(CaptionsParser *p);
void parserSignalAbort(CaptionsParser *p);
void parserGetError(int errCode, wchar_t *buf, int bufSize);
int parserWriteout(CaptionsParser *p);
void parserClose(CaptionsParser *p);

#endif


